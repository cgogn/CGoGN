#ifndef _TEXTURE_H_
#define _TEXTURE_H_

struct Texture
{
	Texture(GLuint id, const QSize& s, unsigned int r) :
		texID(id),
		size(s),
		ref(r)
	{}

	GLuint texID;
	QSize size;
	unsigned int ref;
};

#endif
