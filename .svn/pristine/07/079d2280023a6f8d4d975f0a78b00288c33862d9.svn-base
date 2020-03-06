// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _GEOMETRYTESTUTILS_H_
#define _GEOMETRYTESTUTILS_H_

#include <frams/model/model.h>
#include <frams/model/modelparts.h>
#include <frams/util/3d.h>
#include <frams/util/sstring.h>

/**
 * @brief Geometry module testing utilities.
 * @details Contains functions frequently used by tests, such as loading or generating random Model,
 *     or adding markers of points to a Model.
 * @author Radosław Gołębiewski
 */
namespace GeometryTestUtils
{
	/**
	 * @brief Execution entry point for test based on specified input model.
	 * @details Parses call arguments and performs one of following tasks: printing usage message;
	 *     printing list of genotypes in specified file; creating Model from loaded genotype and
	 *     executing test; creating random Model and executing test.
	 * @param[in] header Header of usage message. This text should contain test description.
	 * @param[in] argc Number of call arguments.
	 * @param[in] argv Call arguments.
	 * @param[in] test Pointer to function implementing test.
	 * @returns Result code: 0 - no error, 1 - incorrect arguments (usage message printed),
	 *     2 - cannot open file, 3 - cannot find genotype, 4 - incorrect genotype.
	 */
	int execute(const SString header, int argc, char *argv[], void (*test)(Model &));
	
	/**
	 * @brief Execution entry point for test based on specified input model and density.
	 * @details Parses call arguments and performs one of following tasks: printing usage message;
	 *     printing list of genotypes in specified file; creating Model from loaded genotype and
	 *     executing test using specified density; creating random Model and executing test using
	 *     specified density.
	 * @param[in] header Header of usage message. This text should contain test description.
	 * @param[in] argc Number of call arguments.
	 * @param[in] argv Call arguments.
	 * @param[in] test Pointer to function implementing test.
	 * @returns Result code: 0 - no error, 1 - incorrect arguments (usage message printed),
	 *     2 - cannot open file, 3 - cannot find genotype, 4 - incorrect genotype.
	 */
	int execute(const SString header, int argc, char *argv[], void (*test)(Model &, const double));
	
	/**
	 * @brief Adds anchor to the specified Model.
	 * @details An anchor has two functions. First is to provide Model consistency. Some functions in
	 *     GeometryTestUtils namespace requires Model passed to them as an argument to contain at
	 *     least one Part. All new Parts are bonded to the rest of Model using Joint connecting them
	 *     with first Part of Model. Second is to provide reference which helps to understand Model
	 *     position, scale and orientation. Anchor is built from four Parts: small sphere placed in
	 *     global coordinate system origin and three cuboids visualising global coordinate system
	 *     axes.
	 * @see addAxesToModel.
	 * @param[in] model Owner of Parts to be created.
	 */
	void addAnchorToModel(Model &model);
	
	/**
	 * @brief Adds point marker to Model.
	 * @details Marker of point is a small sphere (radius = 0.05).
	 * @param[in] point Location of marker.
	 * @param[in] model Owner of Part to be created, must contain at least one part.
	 */
	void addPointToModel(const Pt3D &point, Model &model);
	
	/**
	 * @brief Adds axes markers to Model.
	 * @details Axes markers are three streched (one of scales = 0.5, others = 0.05) and colored
	 *     cuboids. Cuboid visualising OX axis is red, OY - green, and OZ - blue.
	 * @param[in] sizes Axes visual lengths.
	 * @param[in] axes Axes orientation, relatively to global coordinate system axes.
	 * @param[in] center Axes intersection point, relatively to global coordinate system origin.
	 * @param[in] model Owner of Parts to be created, must contain at least one part.
	 */
	void addAxesToModel(const Pt3D &sizes, const Orient &axes, const Pt3D &center, Model &model);
	
	/**
	 * @brief Merges two Models.
	 * @details Moves all parts from source Model to target Model and - to provide Model
	 *     consistency - creates Joint between firsts Parts of each of them. Each model must contain
	 *     at least one Part.
	 * @param[in] target Target Model, must contain at least one part.
	 * @param[in] source Source Model, must contain at least one part.
	 */
	void mergeModels(Model &target, Model &source);
	
	/**
	 * @brief Randomizes position, scale and rotations of Part.
	 * @details Sets coords of Part position to random values from range (1.5, 2.5), scales to
	 *     random values from range (0.1, 1.0), and rotations around each axis to random values from
	 *     range (0, M_PI).
	 * @param[in] part Part which position, scale and orient should be randomized.
	 */
	void randomizePositionScaleAndOrient(Part *part);
	
	/**
	 * @brief Prints description of given Part to specified file.
	 * @details Prints shape ("shape"), position ("x", "y", "z"), scale ("sx", "sy", "sz") and
	 *     rotations ("rx", "ry", "rz") of given Part. Each value is printed in separate line using
	 *     following syntax: "# NAME=VALUE".
	 * @param[in] part Part to be described.
	 * @param[in] output Output file.
	 */
	void describePart(const Part *part, FILE *output);
}

#endif
