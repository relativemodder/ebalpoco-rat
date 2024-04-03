#pragma once
#include <Windows.h>
#include <WinInet.h>
#include <string>

void ScreenShot(char* BmpName)
{
	HWND DesktopHwnd = GetDesktopWindow();
	RECT DesktopParams;
	HDC DevC = GetDC(DesktopHwnd);
	GetWindowRect(DesktopHwnd, &DesktopParams);
	DWORD Width = DesktopParams.right - DesktopParams.left;
	DWORD Height = DesktopParams.bottom - DesktopParams.top;

	DWORD FileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBTRIPLE) + 1 * (Width * Height * 4));
	char* BmpFileData = (char*)GlobalAlloc(0x0040, FileSize);

	PBITMAPFILEHEADER BFileHeader = (PBITMAPFILEHEADER)BmpFileData;
	PBITMAPINFOHEADER  BInfoHeader = (PBITMAPINFOHEADER)&BmpFileData[sizeof(BITMAPFILEHEADER)];

	BFileHeader->bfType = 0x4D42; // BM
	BFileHeader->bfSize = sizeof(BITMAPFILEHEADER);
	BFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	BInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
	BInfoHeader->biPlanes = 1;
	BInfoHeader->biBitCount = 24;
	BInfoHeader->biCompression = BI_RGB;
	BInfoHeader->biHeight = Height;
	BInfoHeader->biWidth = Width;

	RGBTRIPLE* Image = (RGBTRIPLE*)&BmpFileData[sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)];
	RGBTRIPLE color;

	HDC CaptureDC = CreateCompatibleDC(DevC);
	HBITMAP CaptureBitmap = CreateCompatibleBitmap(DevC, Width, Height);
	SelectObject(CaptureDC, CaptureBitmap);
	BitBlt(CaptureDC, 0, 0, Width, Height, DevC, 0, 0, SRCCOPY | CAPTUREBLT);
	GetDIBits(CaptureDC, CaptureBitmap, 0, Height, Image, (LPBITMAPINFO)BInfoHeader, DIB_RGB_COLORS);

	DWORD Junk;
	HANDLE FH = CreateFileA(BmpName, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
	WriteFile(FH, BmpFileData, FileSize, &Junk, 0);
	CloseHandle(FH);
	GlobalFree(BmpFileData);
}

std::string generateRandomPathToAnyFile(std::string extension) {
	// Get the temporary directory path
	std::string tempDir = std::getenv("TEMP");

	// Generate a random name for the file
	std::string randomName;
	std::srand(std::time(nullptr)); // Seed the random number generator
	const std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	const int nameLength = 10; // Length of the random name
	for (int i = 0; i < nameLength; ++i) {
		randomName += alphabet[std::rand() % alphabet.length()];
	}

	// Create the full path to the file
	std::string filePath = tempDir + "/" + randomName + extension;

	return filePath;
}

std::string generateRandomPathToBmpFile() {
	return generateRandomPathToAnyFile(".bmp");
}

size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}

std::vector<std::string> splitString(const std::string& str, char delimiter) {
	std::vector<std::string> splittedStrings;
	std::stringstream ss(str);
	std::string item;
	while (std::getline(ss, item, delimiter)) {
		splittedStrings.push_back(item);
	}
	return splittedStrings;
}

bool downloadFile(const std::string& url, const std::string& destination) {
	HINTERNET hInternetSession = InternetOpen("YourApp", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hInternetSession == NULL) {
		return false;
	}

	HINTERNET hURL = InternetOpenUrlA(hInternetSession, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
	if (hURL == NULL) {
		InternetCloseHandle(hInternetSession);
		return false;
	}

	DWORD bytesRead;
	char buffer[1024];
	HANDLE hFile = CreateFileA(destination.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		InternetCloseHandle(hURL);
		InternetCloseHandle(hInternetSession);
		return false;
	}

	while (InternetReadFile(hURL, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
		DWORD bytesWritten;
		if (!WriteFile(hFile, buffer, bytesRead, &bytesWritten, NULL)) {
			CloseHandle(hFile);
			InternetCloseHandle(hURL);
			InternetCloseHandle(hInternetSession);
			return false;
		}
	}

	CloseHandle(hFile);
	InternetCloseHandle(hURL);
	InternetCloseHandle(hInternetSession);
	return true;
}