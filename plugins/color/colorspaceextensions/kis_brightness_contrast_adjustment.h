/*
 *  SPDX-FileCopyrightText: 2026 Krita Mobile contributors
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef _KIS_BRIGHTNESS_CONTRAST_ADJUSTMENT_H_
#define _KIS_BRIGHTNESS_CONTRAST_ADJUSTMENT_H_

#include "KoColorTransformationFactory.h"

class KisBrightnessContrastAdjustmentFactory : public KoColorTransformationFactory
{
public:

   KisBrightnessContrastAdjustmentFactory();

   QList< QPair< KoID, KoID > > supportedModels() const override;

   KoColorTransformation* createTransformation(const KoColorSpace* colorSpace, QHash<QString, QVariant> parameters) const override;

};

#endif

