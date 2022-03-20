#ifndef _XMSG_TEXTURE_H
#define _XMSG_TEXTURE_H

#include<glad\gl.h>
#include <cstddef>
#include <string>

namespace XMsg {
	namespace Gfx {
		class Texture {
			int width, height;
			GLuint Id = NULL;

		public:

			void CreateFromData(const unsigned char* Data, int nrChannels, int width, int height);

			bool LoadFromFile(const std::string& FileName);

			GLuint GetId();

			int GetWidth();
			int GetHeight();

			~Texture();

		};
	}
}

#endif