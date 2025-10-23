

#include <Arduino.h>
#include <AnimatedGIF.h>
#include <vector>
#include "LittleFS.h"
#include "ui.h"

#include "Drivers/screen_driver.h"
#include "user_config.h"
#include "gif_view.h"

#define DISPLAY_WIDTH TFT_HOR_RES
#define DISPLAY_HEIGHT TFT_VER_RES
#define BUFFER_SIZE 256 // Optimum is >= GIF width or integral division of width

// Static variables
static uint16_t usTemp[2][BUFFER_SIZE]; // Double buffer
static bool dmaBuf = 0;
static AnimatedGIF gif;
static std::vector<String> gifFiles;
static QueueHandle_t gifQueue = nullptr;

File f;

static void *GIFOpenFile(const char *fname, int32_t *pSize)
{
	f = LittleFS.open(fname);
	if (f)
	{
		*pSize = f.size();
		return (void *)&f;
	}
	return NULL;
} /* GIFOpenFile() */

static void GIFCloseFile(void *pHandle)
{
	File *f = static_cast<File *>(pHandle);
	if (f != NULL)
		f->close();
} /* GIFCloseFile() */

static int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
	int32_t iBytesRead;
	iBytesRead = iLen;
	File *f = static_cast<File *>(pFile->fHandle);
	// Note: If you read a file all the way to the last byte, seek() stops working
	if ((pFile->iSize - pFile->iPos) < iLen)
		iBytesRead = pFile->iSize - pFile->iPos - 1; // <-- ugly work-around
	if (iBytesRead <= 0)
		return 0;
	iBytesRead = (int32_t)f->read(pBuf, iBytesRead);
	pFile->iPos = f->position();
	return iBytesRead;
} /* GIFReadFile() */

static int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition)
{
	int i = micros();
	File *f = static_cast<File *>(pFile->fHandle);
	f->seek(iPosition);
	pFile->iPos = (int32_t)f->position();
	i = micros() - i;
	//  Serial.printf("Seek time = %d us\n", i);
	return pFile->iPos;
}

static void GIFDraw(GIFDRAW *pDraw)
{
	uint8_t *s;
	uint16_t *d, *usPalette;
	int x, y, iWidth, iCount;

	// Displ;ay bounds chech and cropping
	iWidth = pDraw->iWidth;
	if (iWidth + pDraw->iX > DISPLAY_WIDTH)
		iWidth = DISPLAY_WIDTH - pDraw->iX;
	usPalette = pDraw->pPalette;
	y = pDraw->iY + pDraw->y; // current line
	if (y >= DISPLAY_HEIGHT || pDraw->iX >= DISPLAY_WIDTH || iWidth < 1)
		return;

	// Old image disposal
	s = pDraw->pPixels;
	if (pDraw->ucDisposalMethod == 2) // restore to background color
	{
		for (x = 0; x < iWidth; x++)
		{
			if (s[x] == pDraw->ucTransparent)
				s[x] = pDraw->ucBackground;
		}
		pDraw->ucHasTransparency = 0;
	}

	// Apply the new pixels to the main image
	if (pDraw->ucHasTransparency) // if transparency used
	{
		uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
		pEnd = s + iWidth;
		x = 0;
		iCount = 0; // count non-transparent pixels
		while (x < iWidth)
		{
			c = ucTransparent - 1;
			d = &usTemp[0][0];
			while (c != ucTransparent && s < pEnd && iCount < BUFFER_SIZE)
			{
				c = *s++;
				if (c == ucTransparent) // done, stop
				{
					s--; // back up to treat it like transparent
				}
				else // opaque
				{
					*d++ = usPalette[c];
					iCount++;
				}
			} // while looking for opaque pixels
			if (iCount) // any opaque pixels?
			{
				// DMA would degrtade performance here due to short line segments
				Screen.drawRect(usTemp[0], pDraw->iX + x, y, iCount, 1);
				x += iCount;
				iCount = 0;
			}
			// no, look for a run of transparent pixels
			c = ucTransparent;
			while (c == ucTransparent && s < pEnd)
			{
				c = *s++;
				if (c == ucTransparent)
					x++;
				else
					s--;
			}
		}
	}
	else
	{
		s = pDraw->pPixels;

		// Unroll the first pass to boost DMA performance
		// Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
		if (iWidth <= BUFFER_SIZE)
			for (iCount = 0; iCount < iWidth; iCount++)
				usTemp[dmaBuf][iCount] = usPalette[*s++];
		else
			for (iCount = 0; iCount < BUFFER_SIZE; iCount++)
				usTemp[dmaBuf][iCount] = usPalette[*s++];
		Screen.drawRect(&usTemp[dmaBuf][0], pDraw->iX, y, iCount, 1);
		dmaBuf = !dmaBuf;
		iWidth -= iCount;
		// Loop if pixel buffer smaller than width
		while (iWidth > 0)
		{
			// Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
			if (iWidth <= BUFFER_SIZE)
				for (iCount = 0; iCount < iWidth; iCount++)
					usTemp[dmaBuf][iCount] = usPalette[*s++];
			else
				for (iCount = 0; iCount < BUFFER_SIZE; iCount++)
					usTemp[dmaBuf][iCount] = usPalette[*s++];
			Screen.drawRect(&usTemp[dmaBuf][0], pDraw->iX, y, iCount, 1);
			dmaBuf = !dmaBuf;
			iWidth -= iCount;
		}
	}
}

static void scan_gif_files()
{
	gifFiles.clear();

	if (!LittleFS.begin(false))
	{
		Serial.println("❌ LittleFS not mounted!");
		return;
	}

	if (!LittleFS.exists("/gif"))
	{
		Serial.println("❌ Folder /gif/ not found!");
		return;
	}

	File root = LittleFS.open("/gif");
	if (!root || !root.isDirectory())
	{
		Serial.println("❌ Cannot open /gif/ or not a directory");
		return;
	}

	File file = root.openNextFile();
	while (file)
	{
		String path = file.name(); // Đã có dạng đầy đủ: "/gif/xxx.gif"
		if (!file.isDirectory())
		{
			if (path.endsWith(".gif") || path.endsWith(".GIF"))
			{
				gifFiles.push_back(path);
				Serial.printf("📁 Found GIF: %s\n", path.c_str());
			}
		}
		file = root.openNextFile();
	}

	Serial.printf("✅ Total GIF files found: %d\n", gifFiles.size());
}

static void show_gif(const char *path)
{
	lv_obj_add_flag(lv_scr_act(), LV_OBJ_FLAG_HIDDEN);
	if (gif.open(path, GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw))
	{
		while (gif.playFrame(true, NULL))
			yield();
		gif.close();
	}
	lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_HIDDEN);
}

void gif_task(void *pvParameters)
{
	gif.begin(LITTLE_ENDIAN_PIXELS);
	if (gifFiles.empty())
		scan_gif_files();
	char filename[64];

	while (true)
	{
		if (xQueueReceive(gifQueue, &filename, portMAX_DELAY) == pdPASS)
		{
			currentMode = UI_MODE_GIF;
			if (xSemaphoreTake(displayMutex, pdMS_TO_TICKS(100)) == pdTRUE)
			{
				Screen.getScreen()->fillScreen(BLACK);
				Serial.printf("Playing GIF: %s\n", filename);
				show_gif(filename);
				xSemaphoreGive(displayMutex);
			}

			currentMode = UI_MODE_ODO;
		}
	}
}

void gif_view_init()
{
	gif.begin(LITTLE_ENDIAN_PIXELS);
	if (gifFiles.empty())
	{
		scan_gif_files();
	}
	gifQueue = xQueueCreate(5, sizeof(char[64]));
	xTaskCreatePinnedToCore(gif_task, "gif_task", 8192, NULL, configMAX_PRIORITIES, NULL, 0);
}

void gif_request_show(const char *filename)
{
	if (gifQueue == nullptr)
		return;
	char name[64];
	strncpy(name, filename, sizeof(name) - 1);
	xQueueSend(gifQueue, &name, 0);
}