#include "Texture.h"
#include <cassert>
#include "..//stb_image/stb_image.h"
#include <iostream>

GLint NrChannelsInInternalFormat(int NrChannels) {
    switch (NrChannels){
    case 3:
        return GL_RGB8;
 
    case 4:
        return GL_RGBA8;

    default:
        std::cout << "NrChannel dont supported:" << NrChannels << std::endl;
        break;
    }
}

GLint NrChannelsInDataFormat(int NrChannels) {
    switch (NrChannels) {
    case 3:
        return GL_RGB;

    case 4:
        return GL_RGBA;

    default:
        std::cout << "NrChannel dont supported:" << NrChannels << std::endl;
        break;
    }
}

void XMsg::Gfx::Texture::CreateFromData(const unsigned char* Data, int nrChannels, int width, int height){
    assert(Id == 0);

    this->width = width;
    this->height = height;

    glGenTextures(1, &Id);

    glBindTexture(GL_TEXTURE_2D, Id);

    glTextureParameteri(Id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(Id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, NrChannelsInInternalFormat(nrChannels), width, height, 0, NrChannelsInDataFormat(nrChannels), GL_UNSIGNED_BYTE, Data);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

bool XMsg::Gfx::Texture::LoadFromFile(const std::string& FileName){
    int width, height, nrChannels;

    unsigned char* Data = stbi_load(FileName.c_str(), &width, &height, &nrChannels, 0);
    if (Data == NULL) {
        return false;
    }

    CreateFromData(Data, width, nrChannels, height);

    stbi_image_free(Data);

    return true;
}

GLuint XMsg::Gfx::Texture::GetId(){
    return Id;
}

int XMsg::Gfx::Texture::GetWidth(){
    return width;
}

int XMsg::Gfx::Texture::GetHeight(){
    return height;
}

XMsg::Gfx::Texture::~Texture(){
    if (Id != NULL) {
        glDeleteTextures(1, &Id);
    }
}
