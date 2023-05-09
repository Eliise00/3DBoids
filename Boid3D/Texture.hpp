#ifndef _TEXTURE_HPP
#define _TEXTURE_HPP

class Texture {
private:
    GLuint _textureID;

public:
    explicit Texture(const std::filesystem::path& filePath, GLuint textureID)
        : _textureID(textureID)
    {
        const auto textureCube = p6::load_image_buffer(filePath);

        glGenTextures(1, &_textureID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureCube.width(), textureCube.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureCube.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLuint getTextureID() const
    {
        return _textureID;
    }
};

#endif // _TEXTURE_HPP
