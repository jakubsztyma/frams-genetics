// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _MESHBUILDER_H_
#define _MESHBUILDER_H_

#include <frams/model/model.h>
#include <frams/model/modelparts.h>
#include <frams/util/3d.h>
#include <frams/util/list.h>
#include "geometryutils.h"

namespace MeshBuilder
{
	class Callback
	{
		public:
			virtual void operator()(const Pt3D &point) = 0;
	};
	
	class Iterator
	{
		protected:
			double density;
		public:
			Iterator(const double _density);
			double getDensity() const;
			void setDensity(const double _density);
			virtual bool tryGetNext(Pt3D &point) = 0;
			void forEach(Callback &callback);
			void addAllPointsToList(SListTempl<Pt3D> &list);
	};

	class Segment: public Iterator
	{
		private:
			Pt3D point1, point2;
			int i, I, numberOfPoints;
		public:
			Segment(const double _density);
			void initialize(const Pt3D &point1, const Pt3D &point2, const bool skipFirst = false, const bool skipLast = false, const bool forceOddNumberOfPoints = false);
			bool tryGetNext(Pt3D &point);
	};

	class RectangleSurface: public Iterator
	{
		private:
			Segment edge1, edge2, area;
			bool skipVerticalEdges, forceOddNumberOfPoints;
		public:
			RectangleSurface(const double _density);
			void initialize(const Part *part, const CuboidFaces::Face face, const bool skipVerticalEdges = false, const bool skipHorizontalEdges = false, const bool forceOddNumberOfPoints = false);
			void initialize(const double width, const double height, const Pt3D &position, const Orient &orient, const bool skipVerticalEdges = false, const bool skipHorizontalEdges = false, const bool forceOddNumberOfPoints = false);
			void initialize(const Pt3D &apex1, const Pt3D &apex2, const Pt3D &apex3, const Pt3D &apex4, const bool skipVerticalEdges = false, const bool skipHorizontalEdges = false, const bool forceOddNumberOfPoints = false);
			bool tryGetNext(Pt3D &point);
	};

	class CuboidApices: public Iterator
	{
		private:
			Pt3D scale;
			Pt3D position;
			Orient orient;
			Octants::Octant octant;
		public:
			CuboidApices();
			void initialize(const Part *part);
			void initialize(const Pt3D &scale, const Pt3D &position, const Orient &orient);
			bool tryGetNext(Pt3D &point);
	};

	class CuboidSurface: public Iterator
	{
		private:
			CuboidApices apices;
			RectangleSurface rectangle;
			Iterator *iterator;
			const Part *part;
			CuboidFaces::Face face;
		public:
			CuboidSurface(const double _density);
			void initialize(const Part *part);
			bool tryGetNext(Pt3D &point);
	};

	class EllipseSurface: public Iterator
	{
		private:
			RectangleSurface rectangle;
			Pt3D position;
			Orient orient;
			double inversedSquaredWidth, inversedSquaredHeight;
		public:
			EllipseSurface(const double _density);
			void initialize(const Part *part, const CylinderBases::Base base);
			void initialize(const double width, const double height, const Pt3D &position, const Orient &orient);
			bool tryGetNext(Pt3D &point);
	};

	class Ellipse: public Iterator
	{
		private:
			enum Phase { yToZ, zToY, Done };
			Phase phase;
			Pt3D position;
			Orient orient;
			double width, height, d, p, q, P, Q, D, a, b;
			QuadrantsYZ::QuadrantYZ quadrant;
		public:
			Ellipse(const double _density);
			void initialize(const Part *part, const CylinderBases::Base base);
			void initialize(const double width, const double height, const Pt3D &position, const Orient &orient);
			bool tryGetNext(Pt3D &point);
		private:
			void calculateAndSetSegmentationDistance();
			void initializePhase(Phase ph);
			void setPoint(Pt3D &point);
			void findNextPQAndPhase();
			int findLastPQOfPhase(Phase ph, double &lp, double &lq);
	};

	class CylinderEdges: public Iterator
	{
		private:
			Ellipse ellipse;
			Pt3D edge, length;
			CylinderBases::Base base;
		public:
			CylinderEdges(const double _density);
			void initialize(const Part *part);
			void initialize(const Pt3D &scale, const Pt3D &position, const Orient &orient);
			void initialize(const double length, const double width, const double height, const Pt3D &position, const Orient &orient);
			bool tryGetNext(Pt3D &point);
	};

	class CylinderWallSurface: public Iterator
	{
		private:
			Ellipse edge;
			Pt3D length;
			Segment area;
		public:
			CylinderWallSurface(const double _density);
			void initialize(const Part *part);
			void initialize(const Pt3D &scale, const Pt3D &position, const Orient &orient);
			void initialize(const double length, const double width, const double height, const Pt3D &position, const Orient &orient);
			bool tryGetNext(Pt3D &point);
	};

	class CylinderSurface: public Iterator
	{
		private:
			CylinderWallSurface wall;
			EllipseSurface ellipse;
			Iterator *iterator;
			const Part *part;
			CylinderBases::Base base;
		public:
			CylinderSurface(const double _density);
			void initialize(const Part *part);
			bool tryGetNext(Pt3D &point);
	};

	class EllipsoidSurface: public Iterator
	{
		private:
			enum Phase { X, Y, Z, Done };
			Phase phase;
			Pt3D edge, area, scale, limit;
			double d;
			const Part *part;
			Octants::Octant octant;
		public:
			EllipsoidSurface(const double _density);
			void initialize(const Part *part);
			bool tryGetNext(Pt3D &point);
		private:
			void initializePhase(Phase ph);
			void setPoint(Pt3D &point);
			void proceedToNextOctant();
			void findNextAreaEdgeAndPhase();
	};

	class PartSurface: public Iterator
	{
		private:
			CuboidSurface cuboid;
			CylinderSurface cylinder;
			EllipsoidSurface ellipsoid;
			Iterator *iterator;
		public:
			PartSurface(const double _density);
			void initialize(const Part *part);
			bool tryGetNext(Pt3D &point);
	};

	class ModelSurface: public Iterator
	{
		private:
			PartSurface surface;
			const Model *model;
			int index;
		public:
			ModelSurface(const double _density);
			void initialize(const Model *model);
			bool tryGetNext(Pt3D &point);
	};

	class PartApices: public Iterator
	{
		private:
			CuboidApices cuboid;
			CylinderEdges cylinder;
			EllipsoidSurface ellipsoid;
			Iterator *iterator;
		public:
			PartApices(const double _density);
			void initialize(const Part *part);
			bool tryGetNext(Pt3D &point);
	};

	class ModelApices: public Iterator
	{
		private:
			PartApices surface;
			const Model *model;
			int index;
		public:
			ModelApices(const double _density);
			void initialize(const Model *model);
			bool tryGetNext(Pt3D &point);
	};

	class BoundingBoxVolume: Iterator
	{
		private:
			Segment edge, area, volume;
			Pt3D length, width, height;
		public:
			BoundingBoxVolume(const double _density);
			void initialize(const Model &model);
			void initialize(const Pt3D &lowerBoundary, const Pt3D &upperBoundary);
			bool tryGetNext(Pt3D &point);
	};
};

#endif
