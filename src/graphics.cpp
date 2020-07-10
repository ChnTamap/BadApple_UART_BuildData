#include "graphics.h"

void graphics_drawLine(
	Color *image, unsigned int width, unsigned int height,
	unsigned int sx, unsigned int sy, unsigned int ex, unsigned int ey,
	Color lineColor)
{
	int w, h;
	unsigned int x, y, i;
	unsigned int *v; //�Ա���
	unsigned int sv;
	unsigned int ev;
	unsigned int *f; //�ӱ���
	unsigned int sf;
	unsigned int ef;
	//ȷ���Ա���
	if (ey == sy)
	{
		if (sx > ex)
		{
			x = ex;
			ex = sx;
		}
		else
		{
			x = sx;
		}
		while (x < ex)
		{
			*(image + width * sy + x) = lineColor;
			x++;
		}
		return;
	}
	if (ex == sx)
	{
		if (sy > ey)
		{
			y = ey;
			ey = sy;
		}
		else
		{
			y = sy;
		}
		while (y < ey)
		{
			*(image + width * y + sx) = lineColor;
			y++;
		}
		return;
	}
	w = (ex - sx) / (ey - sy);
	if (w == 0)
	{
		//h > W, YΪ����
		v = &y;
		sv = sy;
		ev = ey;
		f = &x;
		sf = sx;
		ef = ex;
	}
	else
	{
		//X
		v = &x;
		sv = sx;
		ev = ex;
		f = &y;
		sf = sy;
		ef = ey;
	}
	//��ֵ������
	if (sv > ev)
	{
		//����s��e
		w = sv - ev;
		h = sf - ef;
		sv = ev;
		ev += w;
		sf = ef;
		ef += h;
	}
	else
	{
		w = ev - sv;
		h = ef - sf;
	}
	//����
	for (i = 0; i < w; i++)
	{
		*v = sv + i;
		*f = sf + (int)i * h / w;
		*(image + width * y + x) = lineColor;
	}
}