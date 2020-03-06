// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "nn_layout.h"
#include <vector>
#include "common/nonstd_stl.h"
#ifdef __BORLANDC__
 #include <alloc.h> //borland needs for alloc/free
#endif
#if (defined MACOS) | (defined EMSCRIPTEN)
 #include <stdlib.h>
#endif

//#define DB(x) x
#define DB(x)

#if DB(1)+0
#include <assert.h>
#endif

class block;

/** Information about a single element (neuron). There are N einfo objects in an array called "einfo" */
struct einfo
{
	/** Element's owner */
	class block *block;

	/** Integer coordinates (neurons are simply placed in a grid, (x,y) is a grid cell) */
	int x, y;
};

/** Array[0..N-1] - one einfo for each neuron */
static struct einfo* einfo;

/** Array[0..N-1] - initially each neuron resides in its own block. The algorithm merges blocks until one single block is created */
static block **blocks;

/** N = number of neurons */
static int N;

/** Provides neuron connections information and receives the layout result */
static NNLayoutState *nn;

static char *JEDEN = (char*)"1";

/** Block is a group of neurons.
	After "blocking" some neurons, their relative location will not change. */
class block
{
public:

	/** Block's id is its index in the "blocks" array */
	int id;

	/** Block members (neurons), or actually neuron indexes (0..N-1 ints)  */
	std::vector<int> elementy;

	/** Block's bounding box (a rectangle containing all elemens)
		w=maxx-minx+1; h=maxy-miny+1;
		*/
	int w, h;
	int minx, miny, maxx, maxy;

	/** 2d array, w*h cells, indicating if a given (x,y) location is taken.
		This speeds up checking if neurons from two blocks overlap.
		*/
	char *map;

	/** Creating an initial block consisting of a single neuron */
	block(int nr) : id(nr), w(1), h(1), minx(0), miny(0), maxx(0), maxy(0)
	{
		DB(printf("new block(%d)\n", nr));
		dodajelement(nr, 0, 0);
		blocks[id] = this;
		map = JEDEN;
	}

	~block()
	{
		DB(printf("~ block(%d)\n", id));
		blocks[id] = 0;
		zwolnijmape();
	}

	void zwolnijmape(void)
	{
		if (map)
		{
			if (map != JEDEN) free(map);
			map = 0;
		}
	}

	void potrzebnamapa(void)
	{
		if (map) return;
		odtworzmape();
	}

	void odtworzmape(void)
	{
		zwolnijmape();
		w = maxx - minx + 1;
		h = maxy - miny + 1;
		map = (char*)calloc(1, w*h);
		DB(printf("mapa bloku #%d\n", id));
		for (size_t i = 0; i < elementy.size(); i++)
		{
			int e = elementy[i];
			map[w*(einfo[e].y - miny) + (einfo[e].x - minx)] = 1;
		}
		DB(for (int i = 0; i < h; i++){ for (int e = 0; e < w; e++)printf("%c", map[w*i + e] ? '*' : '.'); printf("\n"); })
	}

	/** Add a neuron to a block at location(x,y) */
	void dodajelement(int nr, int x, int y)
	{
		elementy.push_back(nr);
		einfo[nr].x = x;
		einfo[nr].y = y;
		einfo[nr].block = this;
	}
};

static int moznadolaczyc(block *b, block *b2, int dx, int dy)
{
	/* Check if block b2 can be merged with b with b2 shifted by (dx,dy) so no overlap occurs.
	   All coordinates are relative to b->minx/miny
	   */
	int x1, y1, x2, y2; // union rectangle
	x1 = max(0, b2->minx - b->minx + dx);
	x2 = min(b->maxx - b->minx, -b->minx + dx + b2->maxx);
	if (x1 > x2) return 1;
	y1 = max(0, b2->miny - b->miny + dy);
	y2 = min(b->maxy - b->miny, -b->miny + dy + b2->maxy);
	if (y1 > y2) return 1;
	int x, y;
	dx += b2->minx - b->minx; //dx,dy relative to minx,miny
	dy += b2->miny - b->miny;
	b->potrzebnamapa();
	b2->potrzebnamapa();
	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
			if (b->map[b->w*y + x] && b2->map[b2->w*(y - dy) + (x - dx)]) return 0;
	}
	return 1;
}

/** Merge b2 with b shifting b2 by (dx,dy) - adds all b2's neurons to b and deletes b2 */
static int dolaczblock(block *b, block *b2, int dx, int dy)
{ // return 1 if successful
	if (!moznadolaczyc(b, b2, dx, dy)) return 0; // merging causes no collision
	DB(printf("#%d(%d,%d,%d,%d) + #%d(%d,%d,%d,%d)<%d,%d>", b->id, b->minx, b->miny, b->maxx, b->maxy, b2->id, b2->minx, b2->miny, b2->maxx, b2->maxy, dx, dy));

	b->zwolnijmape();
	for (size_t i = 0; i < b2->elementy.size(); i++)
	{
		int e = b2->elementy[i];
		b->dodajelement(e, einfo[e].x + dx, einfo[e].y + dy);
	}
	b->minx = min(b->minx, dx + b2->minx);
	b->miny = min(b->miny, dy + b2->miny);
	b->maxx = max(b->maxx, dx + b2->maxx);
	b->maxy = max(b->maxy, dy + b2->maxy);

	DB(
	printf(" -> (%d,%d,%d,%d)\n", b->minx, b->miny, b->maxx, b->maxy);

	printf(" ...#%d...(%d)...", b->id, b->elementy.size());
	for (size_t i = 0; i < b->elementy.size(); i++)
	{
		int e = b->elementy[i];
		assert(einfo[e].x >= b->minx);
			assert(einfo[e].x <= b->maxx);
			assert(einfo[e].y >= b->miny);
			assert(einfo[e].y <= b->maxy);
			printf("(%d)%d,%d ", e, einfo[e].x, einfo[e].y);
	}

	printf("\n")
	);

	delete b2;
	return 1;
}

/** e2 neuron will be connected to e neuron's input:
	- e and e2 belong to different blocks: shift/merge the blocks so e2 is to the left of e
	- same block: nothing can be done
	*/
static void polaczjakowejscie(int e, int e2)
{
	block *b, *b2;
	b = einfo[e].block;
	if (!einfo[e2].block) new block(e2);
	b2 = einfo[e2].block;
	if (b == b2)
	{
		DB(printf("--- b==b2 -> cancel\n"));
		return;
	}
	int dx, dy;
	dx = einfo[e].x - einfo[e2].x;
	dy = einfo[e].y - einfo[e2].y;
	DB(printf("  elem.%d (%d,%d@%d) + elem.%d (%d,%d@%d)...\n", e, einfo[e].x, einfo[e].y, b->id, e2, einfo[e2].x, einfo[e2].y, b2->id));
	if (dolaczblock(b, b2, dx - 1, dy)) return;
	int proba; // retry - increasing the y offset (keeps x offset at -1)
	for (proba = 1;; proba++)
	{
		if (dolaczblock(b, b2, dx - 1, dy - proba)) return;
		if (dolaczblock(b, b2, dx - 1, dy + proba)) return;
	}
}

/** Retrieve the information about neuron e inputs and place the input neurons accordingly
	unless they are already processed
	*/
static void ustawelement(int e)
{
	if (einfo[e].block)
	{
		DB(printf("block#%d exists\n", e));
		return;
	}
	new block(e);
	int we;
	int n = nn->GetInputs(e);
	for (we = 0; we < n; we++)
	{
		int e2 = nn->GetLink(e, we);
		if (e2 < 0) continue;
		if (e == e2) continue;
		ustawelement(e2);
		polaczjakowejscie(e, e2);
	}
}

/**
   The algorithm:
   1. Phase one
    - for each neuron, place its input neurons to the left
      (at relative location (-1,dy), where dy is any number, ideally 0)
    - the neuron's location in a block is never changed after the initial assignment
    - which means that any further connections within a given block are ignored (neurons are already fixed in place)
    - foreign block connections cause block merges, shifting the blocks so the (-1,dy) condition is met
      (which also affects all neurons in the other block, since their relative positions are fixed)
    - the final result is a set of blocks corresponding to the "islands" in the neural network
   2. Phase two
    - "islands" are merged into one final block. Any relative offsets can be used, as their neurons
      are not connected anyway. Here a simple method is used: placing the islands in a vertical stack.
 */
void smartlayout(NNLayoutState *nnlayout)
{
	DB(printf("\n >>>>>>>>> smartlayout <<<<<<<<<<<\n"));
	nn = nnlayout;
	N = nn->GetElements();
	einfo = (struct einfo*)calloc(N, sizeof(struct einfo));
	blocks = (class block**)calloc(N, sizeof(class block*));

	int el;
	for (el = 0; el < N; el++) ustawelement(el);

	DB(printf(" - - merging blocks - -\n"));

	block *first;
	for (el = 0; el < N; el++) if (blocks[el]) { first = blocks[el]; el++; break; }
	while (el<N)
	{
		if ((first->maxx - first->minx)>(first->maxy - first->miny))
		{
			int y = first->maxy + 2;
			int x = first->minx;
			int ex = first->maxx;
			while (el<N)
			{
				if (blocks[el])
				{
					int dx = blocks[el]->maxx - blocks[el]->minx + 2;
					dolaczblock(first, blocks[el], x - blocks[el]->minx, y - blocks[el]->miny);
					x += dx;
					if (x>ex) break;
				}
				el++;
			}
		}
		else
		{
			int x = first->maxx + 2;
			int y = first->miny;
			int ey = first->maxy;
			while (el<N)
			{
				if (blocks[el])
				{
					int dy = blocks[el]->maxy - blocks[el]->miny + 2;
					dolaczblock(first, blocks[el], x - blocks[el]->minx, y - blocks[el]->miny);
					y += dy;
					if (y>ey) break;
				}
				el++;
			}
		}
	}
	/*
	for (;el<N;el++)
	if (blocks[el])
	dolaczblock(first,blocks[el],first->minx-blocks[el]->minx,first->maxy-blocks[el]->miny+1);
	*/
	if (first) // at this stage we have a single block containing all neurons
	{
		DB(printf(" - - setting coordinates - -\n"));
		DB(first->odtworzmape());
		for (size_t i = 0; i < first->elementy.size(); i++)
		{
			el = first->elementy[i];
			nn->SetXYWH(el, einfo[el].x * 70, -einfo[el].y * 70, 50, 50);
		}
		delete first;
	}

	free(blocks);
	free(einfo);
	DB(printf("--------------------------------\n\n"));
}
