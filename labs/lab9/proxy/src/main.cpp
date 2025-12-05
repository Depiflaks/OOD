//
// Created by smmm on 05.12.2025.
//
#include "lib/Drawer.h"

#include "lib/CoW.h"
#include "lib/Drawer.h"
#include "lib/Image.h"
#include "lib/ImageProcessor.h"

#include <fstream>

int main()
{
	auto img = ImportImage("in.ppm");

	// DrawLine(img, { 10, 10 }, { 190, 10 }, 0xFFFFFF);
	// DrawLine(img, { 10, 10 }, { 10, 190 }, 0xFFFFFF);

	// DrawCircle(img, { 80, 80 }, 4, 0xFF0000);
	// DrawCircle(img, { 140, 80 }, 6, 0x00FF00);
	FillCircle(img, { 20, 20 }, 20, 0xFFFFFF);

	SaveImage(img, "out.ppm");
}
