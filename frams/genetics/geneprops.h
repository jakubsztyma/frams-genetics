// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _GENEPROPS_H
#define _GENEPROPS_H

#include <common/nonstd_math.h>
#include <frams/model/model.h>





#define F14_MODIFIERS_VISUAL "DdGgBbHh"
#define F14_MODIFIERS_RARE "EeWwAaSs" //expdef would need to handle these properly/specifically to ensure reasonable behavior, and hardly any expdef does. Modifying initial energy of a creature as a result of its genes (Ee) is in general not a good idea. Weight (Ww) works only in water, and in water sinking/going up should usually be caused by real "intentional" activity of a creature, not by its inherited weight. For assimilation (Aa), there is a dedicated parameter in CreaturesGroup. Stamina (Ss) is no longer needed as destructive collisions are not supported, and even if they were, some expdef would need to impose reasonable restrictions on the value of this parameter (e.g. similar to normalizeBiol4()) so there is some cost associated with it, and the specific consequences of destructions should be defined as needed.
#define F14_MODIFIERS "LlRrCcQqFfMmIi" F14_MODIFIERS_RARE F14_MODIFIERS_VISUAL





/**
 * Contains physical, biological and other properties of
 * stick, except for rotation. The constructor initializes properties of sticks with
 * default values. In order to change a property of a stick, the executeModifier() method
 * should be called. Modification of length, curvedness and twist properties
 * usually affects further sticks, so new sticks should have properties of
 * parents (prop) modified with the prop.propagateAlong() method.
 * "Biological" properties (assimilation, stamina, muscle strength and
 * ingestion) should be normalized after modification with normalizeBiol4().
 */
struct GeneProps
{
public:
	double length;            ///<incremented by L, decremented by l. Physical property, length of stick
	double curvedness;        ///<incremented by C, decremented by c. Curvedness of sticks
	double weight;            ///<incremented by W, decremented by w. Physical property, weight of stick (in water environment light sticks swim on the surface)
	double friction;          ///<incremented by F, decremented by f. Physical property, friction of a stick (sticks will slide on the ground or stick to it)

	double muscle_power;      ///<incremented by M, decremented by m. Biological property, muscle strength (muscle speed). Strong muscles act with bigger force, gain higher speed, can resist bigger stress, and use more energy
	double assimilation;      ///<incremented by A, decremented by a. Biological property, assimilation, photosynthesis (a vertical stick can assimilate twice as much as horizontal one)
	double stamina;           ///<incremented by S, decremented by s. Biological property, stamina (increases chance of survival during fights)
	double ingestion;         ///<incremented by I, decremented by i. Biological property, ingestion (ability to gain energy from food)

	double twist;             ///<incremented by Q, decremented by q. Twist of a stick
	double energy;            ///<incremented by E, decremented by e. Energy of a creature

	double muscle_bend_range; ///<Used only by conv_f1
	bool muscle_reset_range;  ///<Used only by conv_f1

	double visual_size;       ///<incremented by H, decremented by h. Part's visual size, only affects appearance
	double cred;              ///<incremented by D, decremented by d. Part's red color proportion
	double cgreen;            ///<incremented by B, decremented by b. Part's blue color proportion
	double cblue;             ///<incremented by G, decremented by g. Part's green color proportion

	static GeneProps standard_values;

	/**
	 * Constructor initializing all properties with default values.
	 */
	GeneProps();

	/**
	 * Normalizes biological properties (muscle_power,
	 * assimilation, stamina, and ingestion). This method is called in
	 * executeModifier() when any of the biological properties is modified. All values
	 * of those properties sum up to 1.
	 */
	void normalizeBiol4();

	/**
	 * Checks whether the given character is property modifier. If yes, the property
	 * is modified and properties are normalized when needed.
	 * @param modif character that might be a property modifier
	 * @return 0 if the provided character was property modifier, -1 otherwise
	 */
	int executeModifier(char modif);

	/**
	 * Adjusts current properties for the next stick. In order to set
	 * new properties to the created stick, the copy of the previous stick should be created,
	 * and propagateAlong() should be used for that copy.
	 * @param use_reset_range true if this method should modify muscle_bend_range (used in f1 conversion).
	 */
	void propagateAlong(bool use_f1_muscle_reset_range);
};

#endif // _GENEPROPS_H
