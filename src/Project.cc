#include "Project.h"

#include "Action/Action.h"
#include "Constants.h"
#include "Frame.h"

#include <cstdio>
#include <memory>
#include <string.h>

#include <fstream>
#include <iostream>
#include <map>
#include <stack>
#include <type_traits>
#include <vector>

#include "MacroUtils.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <format>

#include <ctime>

#include <stdlib.h>

#define WRITEB(b)                                                        \
	do {                                                                 \
		outFile.write(reinterpret_cast<const char*>(&(b)), sizeof((b))); \
	} while (0)

namespace FuncDoodle {
	ProjectFile::ProjectFile(char name[], int width, int height, char author[],
		int fps, char desc[], Platform::Window* win, Col bgCol)
		: m_Window(win) {
		strcpy(m_Name, name);
		m_Width = width;
		m_Height = height;
		strcpy(m_Author, author);
		m_FPS = fps;
		strcpy(m_Desc, desc);

		m_BG = bgCol;

		m_Frames.reset(new LongIndexArray(width, height, bgCol));
		m_Frames->PushBackEmpty();

		m_UndoStack = std::stack<UniquePtr<Action>>();
		m_RedoStack = std::stack<UniquePtr<Action>>();
	}
	ProjectFile::~ProjectFile() {}

	const char* ProjectFile::AnimName() const {
		return m_Name;
	}
	void ProjectFile::SetAnimName(char name[]) {
		strcpy(m_Name, name);
	}

	void ProjectFile::Export(const char* filePath, int format) {
		FUNC_GRAY("Exporting to " << filePath);

		auto frames = AnimFrames();

		char curFilePath[g_FilePathBufferSize];

		for (unsigned long i = 0; i < AnimFrameCount(); i++) {
#ifndef _WIN32
			snprintf(curFilePath, sizeof(curFilePath), "%s/frame_%lu.png",
				filePath, i);
#else
			snprintf(curFilePath, sizeof(curFilePath), "%s\\frame_%lu.png",
				filePath, i);
#endif
			frames->Get(i)->Export(curFilePath);
		}

		if (format == 1) {
			FUNC_GRAY("Exporting to mp4...");
			// TODO: properly implement video exporting...
			// now that im looking back on this, HOW DID I EVER THINK THIS WAS A
			// GOOD SOLUTION?????????? WAS I STUPID???? apparently yes i was
			// ...
			//
			// if anyone's looking at this comment, please - do NOT use the
			// solution i'm using here its overwhelmingly hacky, breaks when
			// ffmpeg updates, is very horrible for UX
			//
			// i think the only reason i wrote this code was to avoid using a
			// ffmpeg wrapper lib thing but like really..?
			//
			// so yea sorry for that
			// idk if/when i'll fix this
			// see you until then
			char cmd[g_LargeBufferSize];
#ifndef _WIN32
			snprintf(cmd, sizeof(cmd),
				"ffmpeg -framerate %d -pattern_type glob -i \"%s/frame_*.png\" "
				"-c:v libx264 -preset veryslow -crf 0 %s/result.mp4 -y",
				m_FPS, filePath, filePath);
#else
			snprintf(cmd, sizeof(cmd),
				"ffmpeg.exe -framerate %d -pattern_type glob -i "
				"\"%s/frame_*.png\" "
				"-c:v libx264 -preset veryslow -crf 0 %s\\result.mp4 -y",
				m_FPS, filePath, filePath, filePath);
#endif

			system(cmd);
		}

		if (format > 1) {
			FUNC_FATAL("Failed to export animation -- format not yet "
					   "supported, this shouldn't normally occur unless "
					   "there's a bug. Submit a github issue");
		}
	}

	const int ProjectFile::AnimWidth() const {
		return m_Width;
	}

	void ProjectFile::SetAnimWidth(int width, bool clear) {
		for (unsigned long i = 0; i < AnimFrameCount(); ++i) {
			m_Frames->Get(i)->SetWidth(width, clear);
		}
		m_Width = width;
	}

	const int ProjectFile::AnimHeight() const {
		return m_Height;
	}
	void ProjectFile::SetAnimHeight(int height, bool clear) {
		for (unsigned long i = 0; i < AnimFrameCount(); ++i) {
			m_Frames->Get(i)->SetHeight(height, clear);
		}
		m_Height = height;
	}

	const char* ProjectFile::AnimAuthor() const {
		return m_Author;
	}
	void ProjectFile::SetAnimAuthor(char* author) {
		strcpy(m_Author, author);
	}

	const int ProjectFile::AnimFPS() const {
		return m_FPS;
	}
	void ProjectFile::SetAnimFPS(int FPS) {
		m_FPS = FPS;
	}

	const char* ProjectFile::AnimDesc() const {
		return m_Desc;
	}
	void ProjectFile::SetAnimDesc(char* desc) {
		strcpy(m_Desc, desc);
	}

	const unsigned long ProjectFile::AnimFrameCount() const {
		return m_Frames->Size();
	}
	SharedPtr<LongIndexArray> ProjectFile::AnimFrames() {
		return m_Frames;
	}

	void ProjectFile::Undo() {
		if (m_UndoStack.empty()) {
			FUNC_INF("Nothing to undo");
			return;
		}

		m_Saved = false;

		UniquePtr<Action> action = std::move(m_UndoStack.top());
		m_UndoStack.pop();

		action->Undo();

		m_RedoStack.push(std::move(action));
	}

	void ProjectFile::Redo() {
		if (m_RedoStack.empty()) {
			FUNC_INF("Nothing to redo");
			return;
		}
		m_Saved = false;
		UniquePtr<Action> action = std::move(m_RedoStack.top());
		m_RedoStack.pop();

		action->Redo();

		m_UndoStack.push(std::move(action));
	}

	void ProjectFile::Write(const char* fileName) {
		unsigned char null = 0;

		std::ofstream outFile(fileName, std::ios::binary);
		if (!outFile.is_open()) {
			FUNC_ERR("Failed to open file for writing");
			return;
		}

		outFile << "FDProj";

		// 0.3
		unsigned long frames = m_Frames->Size();

		int major = FDPVERMAJOR;
		int minor = FDPVERMINOR;
		WRITEB(major);
		WRITEB(minor);
		WRITEB(frames);		// frame count (default)
		WRITEB(m_Width);	// animation width
		WRITEB(m_Height);	// animation height
		WRITEB(m_FPS);		// animation fps
		outFile << m_Name;	// animation name
		WRITEB(null);
		outFile << m_Desc;	// animation description
		WRITEB(null);
		outFile << m_Author;  // animation author
		WRITEB(null);
		WRITEB(m_BG.r);
		WRITEB(m_BG.g);
		WRITEB(m_BG.b);
		WRITEB(null);

		auto frameData = AnimFrames();

		// Use a vector and maintain stable indices
		std::vector<Col> uniqueColors;
		std::map<Col, int> colorToIndex;  // Map each color to its stable index

		// First pass: collect unique colors with stable ordering
		for (unsigned long i = 0; i < AnimFrameCount(); i++) {
			auto pixels = frameData->Get(i)->Pixels();
			for (int x = 0; x < pixels->Width(); x++) {
				for (int y = 0; y < pixels->Height(); y++) {
					Col px = pixels->Get(x, y);
					if (colorToIndex.find(px) == colorToIndex.end()) {
						colorToIndex[px] = uniqueColors.size();
						uniqueColors.push_back(px);
					}
				}
			}
		}

		if (count(uniqueColors.begin(), uniqueColors.end(), m_BG) < 0) {
			uniqueColors.push_back(m_BG);
		}

		// Write palette size
		std::size_t plteLen = uniqueColors.size();
		WRITEB(plteLen);

		// Write palette colors
		for (const Col& col : uniqueColors) {
			WRITEB(col.r);
			WRITEB(col.g);
			WRITEB(col.b);
		}

		// Write frame data using stable indices
		for (unsigned long i = 0; i < AnimFrameCount(); i++) {
			auto pixels = frameData->Get(i)->Pixels();
			for (int y = 0; y < pixels->Height(); y++) {
				for (int x = 0; x < pixels->Width(); x++) {
					Col px = pixels->Get(x, y);
					int index = colorToIndex[px];
					WRITEB(index);
				}
			}
			unsigned char null = 0;
			WRITEB(null);
		}

		outFile << "EOP";
		outFile.close();
		m_Saved = true;
		m_LastSavePath = fileName;
	}
	void ProjectFile::ReadAndPopulate(const char* filePath) {
		std::ifstream file(filePath, std::ios::in | std::ios::binary);

		if (!file.is_open()) {
			FUNC_FATAL("Failed to open file");
			return;
		}

		const int numBytes = 6;
		std::string str(numBytes, '\0');

		file.read(&str[0], numBytes);

		if (str != "FDProj") {
			FUNC_FATAL("This isn't a funcdoodle project...");
		}

		m_UndoStack = std::stack<UniquePtr<Action>>();
		m_RedoStack = std::stack<UniquePtr<Action>>();

		int verMajor = 0;
		file.read(reinterpret_cast<char*>(&verMajor), sizeof(verMajor));
		int verMinor = 0;
		file.read(reinterpret_cast<char*>(&verMinor), sizeof(verMinor));

		unsigned long frameCount = 0;  // temp val
		file.read(reinterpret_cast<char*>(&frameCount), sizeof(frameCount));
		int animWidth = 0;
		file.read(reinterpret_cast<char*>(&animWidth), sizeof(animWidth));
		int animHeight = 0;
		file.read(reinterpret_cast<char*>(&animHeight), sizeof(animHeight));
		int animFPS = 0;
		file.read(reinterpret_cast<char*>(&animFPS), sizeof(animFPS));

		// BRUH
		m_Width = animWidth;
		m_Height = animHeight;

		file.getline(m_Name, sizeof(m_Name), '\0');

		if (file.fail()) {
			FUNC_FATAL("Failed to read file");
		}

		file.getline(m_Desc, sizeof(m_Desc), '\0');

		file.getline(m_Author, sizeof(m_Author), '\0');

		unsigned char bgR = g_MaxColorValue;
		unsigned char bgG = g_MaxColorValue;
		unsigned char bgB = g_MaxColorValue;

		if (verMajor >= 0 && verMinor >= 1) {
			file.read(reinterpret_cast<char*>(&bgR), sizeof(bgR));
			file.read(reinterpret_cast<char*>(&bgG), sizeof(bgG));
			file.read(reinterpret_cast<char*>(&bgB), sizeof(bgB));
			unsigned char null;
			file.read(reinterpret_cast<char*>(&null), 1);
		} else {
			if (verMajor != FDPVERMAJOR && verMinor != FDPVERMINOR) {
				verMinor++;
				if (verMinor >= 10) {
					verMinor = 0;
					verMajor++;
				}
			}
		}

		std::vector<Col> plte;

		std::size_t plteLen = 0;

		if (file.fail()) {
			FUNC_FATAL("Failed to read file");
		}

		file.read(reinterpret_cast<char*>(&plteLen), sizeof(plteLen));
		FUNC_DBG("plteLen = " << plteLen);

		if (file.fail()) {
			FUNC_FATAL("Failed to read file");
		}

		for (std::size_t i = 0; i < plteLen; i++) {
			// read the rgb
			unsigned char r = 0;
			unsigned char g = 0;
			unsigned char b = 0;
			file.read(reinterpret_cast<char*>(&r), sizeof(r));
			file.read(reinterpret_cast<char*>(&g), sizeof(g));
			file.read(reinterpret_cast<char*>(&b), sizeof(b));

			plte.push_back(Col{.r = r, .g = g, .b = b});
		}

		m_Frames.reset(new LongIndexArray(m_Width, m_Height, m_BG));
		if (verMajor >= 0 && verMinor >= 2) {
			FUNC_GRAY("Reading " << (unsigned long)frameCount << "frames...");
			for (unsigned long i = 0; i < (unsigned long)frameCount; i++) {
				ImageArray img(animWidth, animHeight, m_BG);
				for (int y = 0; y < animHeight; y++) {
					for (int x = 0; x < animWidth; x++) {
						std::streampos start = file.tellg();
						unsigned char bytes[sizeof(int)];
						file.read(reinterpret_cast<char*>(bytes), sizeof(int));
						int index = *reinterpret_cast<int*>(bytes);
						if (index >= (int)plteLen) {
							FUNC_DBG("Index -- " << index);
							FUNC_DBG("Index out of bounds -- maybe the project "
									 "file is corrupted..?");
							FUNC_DBG("trying to break...");
							file.seekg(start);
							break;
						}

						img.Set(x, y, plte[index]);
					}
				}
				Frame newFrame = Frame(&img);
				m_Frames->PushBack(&newFrame);

				unsigned char null;
				file.read(reinterpret_cast<char*>(&null), 1);
			}
		} else {
			FUNC_GRAY("Reading " << (long)frameCount << "frames...");
			for (long i = 0; i < (long)frameCount; i++) {
				ImageArray img(animWidth, animHeight, m_BG);
				for (int y = 0; y < animHeight; y++) {
					for (int x = 0; x < animWidth; x++) {
						unsigned char bytes[sizeof(int)];
						file.read(reinterpret_cast<char*>(bytes), sizeof(int));

						int index = *reinterpret_cast<int*>(bytes);

						FUNC_DBG("Reading index, x=" << x << " y=" << y
													 << " index=" << index);

						if (index < 0 || index >= (int)plteLen) {
							FUNC_WARN("Index out of bounds -- maybe the "
									  "project file is corrupted..?");
							FUNC_INF("Index: " << index);
							std::exit(-1);
						}

						img.Set(x, y, plte[index]);
					}
				}
				Frame newFrame = Frame(&img);
				m_Frames->PushBack(&newFrame);
				unsigned char null;
				file.read(reinterpret_cast<char*>(&null), 1);
			}
			if (verMajor != FDPVERMAJOR && verMinor != FDPVERMINOR) {
				verMinor++;
				if (verMinor >= 10) {
					verMinor = 0;
					verMajor++;
				}
			}
		}
		m_Width = animWidth;
		m_Height = animHeight;
		m_FPS = animFPS;
		m_Saved = true;
		// maybe decide if the color space is needed cos everything is sRGB
		// nowadays.

		char eop[4];
		file.getline(eop, sizeof(eop), '\0');

		if (strcmp(eop, "EOP") == 0) {
			FUNC_INF("SUCCESS READING FILE!");
		}

		if (!file) {
			FUNC_FATAL("Failed to read from file");
		}

		file.close();
	}

	void ProjectFile::DisplayAltFPS(double fps) {
		if (ImGui::GetIO().KeyAlt) {
			char* title = (char*)malloc(g_LargeBufferSize);
			snprintf(title, g_LargeBufferSize, "FuncDoodle %s: %s%s (%d FPS)",
				FUNCVER, AnimName(), !m_Saved ? "*" : "",
				(int)(fps > 0.0 ? fps : ImGui::GetIO().Framerate));

			m_Window->SetTitle(title);
			free(title);
		}
	}

	void ProjectFile::UpdateTitle() {
		char* title = (char*)malloc(g_LargeBufferSize);

		snprintf(title, g_LargeBufferSize,
			"FuncDoodle %s: %s%s (alt for details)", FUNCVER, AnimName(),
			!m_Saved ? "*" : "");

		m_Window->SetTitle(title);
		free(title);
	}
}  // namespace FuncDoodle
