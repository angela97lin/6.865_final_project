#ifndef A10_H_PHUDVTKB
#define A10_H_PHUDVTKB
#endif /* end of include guard: A10_H_PHUDVTKB */

#include "Image.h"

void brush(Image &im,
           int x,
           int y,
           std::vector<float> color,
           const Image &texture);

void singleScalePaint(const Image &im,
                      Image &out,
                      const Image &importance,
                      const Image &texture,
                      int size = 10,
                      int strokes = 1000,
                      float noise = 0.3f);

Image sharpnessMap(const Image &im,
                   float sigma = 1.0f,
                   float truncate = 4.0f,
                   bool clamp = true);

Image painterly(const Image &im,
                const Image &texture,
                int strokes = 10000,
                int size = 50,
                float noise = 0.3f);

Image computeTensor(const Image &im,
                    float sigmaG = 3.0f,
                    float factorSigma = 5.0f);

Image computeAngles(const Image &im);

std::vector<Image> rotatedBrushes(const Image &texture,
                                  int numAngles = 36);

void singleScaleOrientedPaint(const Image &im,
                              Image &out,
                              const Image &angles,
                              const Image &importance,
                              const Image &texture,
                              int strokes,
                              int size,
                              float noise,
                              int numAngles = 36);

Image orientedPaint(const Image &im,
                    const Image &texture,
                    int strokes = 7000,
                    int size = 50,
                    float noise = 0.3f);



Image darkToLightPaint(const Image &im,
                       const Image &texture,
                       int strokes = 10000,
                       int size = 50,
                       float noise = 0.3f);

Image lightToDarkPaint(const Image &im,
                       const Image &texture,
                       int strokes = 1000,
                       int size = 50,
                       float noise = 0.3f);

void lightToDarkPaintHelper(const Image &im,
                            Image &out,
                            const Image &angles,
                            const Image &importance,
                            const Image &texture,
                            int strokes = 7000,
                            int size = 50,
                            float noise = 0.3f,
                            int numAngles = 36);

void darkToLightPaintHelper(const Image &im,
                            Image &out,
                            const Image &angles,
                            const Image &importance,
                            const Image &texture,
                            int strokes = 7000,
                            int size = 50,
                            float noise = 0.3f,
                            int numAngles = 36);

Image multiScaleOrientedPaint(const Image &im,
                    const Image &texture,
                    int strokes,
                    int size, float noise, int numScales = 2);