//#include <glad/glad.h>
//
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>
//
////unsigned int texture;
////glad_glGenTextures(1, &texture);
////glBindTexture(GL_TEXTURE_2D, texture);
////// set the texture wrapping/filtering options (on currently bound texture)
////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
////// load and generate the texture
////int width, height, nrChannels;
////unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
////
////if (data)
////{
////	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
////		GL_UNSIGNED_BYTE, data);
////	glGenerateMipmap(GL_TEXTURE_2D);
////}
////else
////{
////	std::cout << "Failed to load texture" << std::endl;
////}
//
