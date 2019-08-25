#include <iostream>
#include <cstdlib>
#include <ctime>
#include "a10.h"
#include "basicImageManipulation.h"
#include "filtering.h"
#include <Eigen/Eigenvalues>

using namespace std;

void brush(Image &im,
           int x,
           int y,
           std::vector<float> color,
           const Image &texture)
{
    // takes as input a mutable image out and splats a single brush stroke centered at x,y
    // out: the image to draw to
    // x,y: where to draw in out
    // r,g,b: the color of the stroke
    // texture: the texture of the stroke, a regular 3-channel image that specifies the opacity at each pixel.
    // It will be greyscale (all three channels are the same) in practice.

    // An opacity of zero means that the pixel is unaffected, and 1 means that the pixel is updated
    // with color. For values between 0 and 1, the output is a linear combination of
    // the old value and color.

    // Ignore cases where y and x are closer than half of the texture size away from the image boundary
    // (in otherwords, your function should do nothing in this case). Plus this will give
    // you the nice rough edges shown in the above image.

    int half_width = texture.width() / 2;
    int half_height = texture.height() / 2;

    // Ignore cases where x and y are closer than half of the texture size away from the image boundary
    if ((x + half_width >= im.width()) || (y + half_height >= im.height()) ||
        (x - half_width < 0) || (y - half_height < 0))
    {
        return;
    }

    for (int i = -half_width; i < half_width; ++i)
    {
        for (int j = -half_height; j < half_height; ++j)
        {
            float opacity = texture(i + half_width, j + half_height);
            for (int c = 0; c < im.channels(); ++c)
            {
                im(x + i, y + j, c) = opacity * color[c] + (1.0f - opacity) * im(x + i, y + j, c);
            }
        }
    }
}

void singleScalePaint(const Image &im,
                      Image &out,
                      const Image &importance,
                      const Image &texture,
                      int size,
                      int strokes,
                      float noise)
{
    // Paints with all brushed at the same scale using importance sampling.

    // First, scale the texture image so that it has maximum size size.
    // For each of N random locations (x, y), splat a brush in out.

    // The color of the brush should be read from im at (x, y). To make things
    // more interesting, modulate the color by multiplying it by a random amount
    // proportional to noise: (1-noise/2+noise*numpy.random.rand(3))

    // first, scale texture image
    float factor = static_cast<float>(size) / max(texture.width(), texture.height());
    Image scaled_texture = scaleLin(texture, factor);

    srand(static_cast<unsigned>(time(0)));

    float average_prob = importance.mean();
    if (average_prob == 0.0f)
    {
        // avoid divide by 0 errors
        // if average_probability = 0, then we always reject.
        return;
    }

    // Since we rejected a number of samples, we do not splat the required N
    // strokes. To fix this, multiply N by a normalization factor based on the
    // average probability of accepting samples.
    int strokes_to_draw = static_cast<int>(static_cast<float>(strokes) / average_prob);
    // cout << "number of strokes to draw:" << strokes_to_draw << endl;
    for (int i = 0; i < strokes_to_draw; ++i)
    {
        // generate random x and y coordinates
        int x = rand() % im.width();
        int y = rand() % im.height();

        // if random value < importance sample value, keep. otherwise, skip.
        float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        if (r <= importance(x, y))
        {
            std::vector<float> color;
            for (int c = 0; c < 3; ++c)
            {
                float n = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                float mod = (1.0f - (noise / 2.0f)) + (noise * n);
                color.push_back(im(x, y, c) * mod);
            }
            brush(out, x, y, color, scaled_texture);
        }
    }
}

Image sharpnessMap(const Image &im,
                   float sigma,
                   float truncate,
                   bool clamp)
{
    // Return image where values correspond to strength of frequencies.
    Image L = color2gray(im);
    Image blur = gaussianBlur_separable(L, sigma, truncate, clamp);
    Image highPass = L - blur;
    Image energy = highPass * highPass;
    Image sharpness = gaussianBlur_separable(energy, 4.0f * sigma);
    Image normalized_sharpness = sharpness / sharpness.max();
    Image three_channel_normalized_sharpness = Image(im.width(), im.height(), im.channels());
    for (int x = 0; x < im.width(); ++x)
    {
        for (int y = 0; y < im.height(); ++y)
        {
            for (int c = 0; c < 3; ++c)
            {
                three_channel_normalized_sharpness(x, y, c) = normalized_sharpness(x, y);
            }
        }
    }
    return three_channel_normalized_sharpness;
}

Image painterly(const Image &im,
                const Image &texture,
                int strokes,
                int size,
                float noise)
{
    // First paints at a coarse scale using all 1's for importance sampling,
    // then paints again at size/4 scale using the sharpness map for importance sampling.
    Image importance = Image(im.width(), im.height(), im.channels());
    for (int x = 0; x < importance.width(); ++x)
    {
        for (int y = 0; y < importance.height(); ++y)
        {
            importance(x, y, 0) = 1.0f;
            importance(x, y, 1) = 1.0f;
            importance(x, y, 2) = 1.0f;
        }
    }

    Image sharpness = sharpnessMap(im);
    Image out(im.width(), im.height(), im.channels());
    Image out2(im.width(), im.height(), im.channels());

    singleScalePaint(im, out, importance, texture, size, strokes, noise);
    singleScalePaint(im, out, sharpness, texture, size / 4, strokes, noise);

    // out2 is for debugging purposes only
    singleScalePaint(im, out2, sharpness, texture, size / 4, strokes, noise);
    out2.debug_write();
    // out2.debug_write();
    return out;
}

Image computeTensor(const Image &im,
                    float sigmaG,
                    float factorSigma)
{
    // // --------- from PS07 ------------------------------
    // Compute xx/xy/yy Tensor of an image. (stored in that order)

    // first, extract the luminance and blur
    std::vector<Image> lc = lumiChromi(im);
    Image lumi = lc[0];
    Image lumi_blurred = gaussianBlur_separable(lumi, sigmaG);
    Image lumi_gradX = gradientX(lumi_blurred);
    Image lumi_gradY = gradientY(lumi_blurred);

    int width = im.width();
    int height = im.height();
    Image tensor(width, height, 3);

    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            float Ix = lumi_gradX(x, y);
            float Iy = lumi_gradY(x, y);

            tensor(x, y, 0) = Ix * Ix;
            tensor(x, y, 1) = Ix * Iy;
            tensor(x, y, 2) = Iy * Iy;
        }
    }

    Image weighted_tensor = gaussianBlur_separable(tensor, sigmaG * factorSigma);
    return weighted_tensor;
}

Image computeAngles(const Image &im)
{
    // Return an image that holds the angle of the smallest eigenvector
    // of the structure tensor at each pixel.
    int width = im.width();
    int height = im.height();
    int channels = im.channels();
    Image response(width, height, channels);
    Image tensor = computeTensor(im);
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            float IxIx = tensor(x, y, 0);
            float IxIy = tensor(x, y, 1);
            float IyIy = tensor(x, y, 2);

            // Change from pset 7:
            // extract eigenvectors + compute angle of the smallest one
            // with respect to horizontal direction

            // construct matrix
            Eigen::Matrix<float, 2, 2> M;
            M << IxIx, IxIy,
                IxIy, IyIy;

            Eigen::EigenSolver<Eigen::Matrix<float, 2, 2>> s(M);
            float e1_x = s.eigenvectors()(0, 0).real();
            float e1_y = s.eigenvectors()(1, 0).real();
            float e2_x = s.eigenvectors()(0, 1).real();
            float e2_y = s.eigenvectors()(1, 1).real();

            float angle1 = atan2(e1_y, e1_x);
            float angle2 = atan2(e2_y, e2_x);
            float angle = angle1;
            // if (abs(s.eigenvalues()(1)) < abs(s.eigenvalues()(0)))
            // {
            //     angle = angle2;
            // }

            if (real(s.eigenvalues()(1)) < real(s.eigenvalues()(0)))
            {
                angle = angle2;
            }
            if (angle < 0)
                angle += 2 * M_PI;

            for (int c = 0; c < channels; ++c)
            {
                response(x, y, c) = angle;
            }
        }
    }
    // response.debug_write();
    return response;
}

std::vector<Image> rotatedBrushes(const Image &texture,
                                  int numAngles)
{
    float N = static_cast<float>(numAngles);
    std::vector<Image> rotated;
    for (int i = 0; i < numAngles; ++i)
    {
        float theta = 2.0f * M_PI / N * static_cast<float>(i);
        Image r = rotate(texture, -theta);
        rotated.push_back(r);
    }
    return rotated;
}

void singleScaleOrientedPaint(const Image &im,
                              Image &out,
                              const Image &angles,
                              const Image &importance,
                              const Image &texture,
                              int strokes,
                              int size,
                              float noise,
                              int numAngles)
{
    // '''same as single scale paint but now the brush strokes will be oriented
    //  according to the angles in angles.'''

    // first, scale texture image
    float factor = static_cast<float>(size) / max(texture.width(), texture.height());
    Image scaled_texture = scaleLin(texture, factor);
    // std::vector<Image> rotated = rotatedBrushes(scaled_texture, numAngles);

    srand(static_cast<unsigned>(time(0)));

    float average_prob = importance.mean();
    if (average_prob == 0.0f)
    {
        // avoid divide by 0 errors
        // if average_probability = 0, then we always reject.
        return;
    }

    // // Since we rejected a number of samples, we do not splat the required N
    // // strokes. To fix this, multiply N by a normalization factor based on the
    // // average probability of accepting samples.
    int strokes_to_draw = static_cast<int>(static_cast<float>(strokes) / average_prob);
    // // cout << "number of strokes to draw:" << strokes_to_draw << endl;
    for (int i = 0; i < strokes_to_draw; ++i)
    {
        // generate random x and y coordinates
        int x = rand() % im.width();
        int y = rand() % im.height();

        // if random value < importance sample value, keep. otherwise, skip.
        float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        if (r <= importance(x, y))
        {
            std::vector<float> color;
            for (int c = 0; c < 3; ++c)
            {
                float n = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                float mod = (1.0f - (noise / 2.0f)) + (noise * n);
                color.push_back(im(x, y, c) * mod);
            }
            float angle = angles(x, y);
            // int index = (angle) * static_cast<float>(numAngles);
            // cout << angle << ", index:" << index << "out of " << rotated.size() << endl;

            Image r = rotate(scaled_texture, angle);

            brush(out, x, y, color, r);
        }
    }
}

Image orientedPaint(const Image &im,
                    const Image &texture,
                    int strokes,
                    int size, float noise)
{
    // Same as painterly but computes and uses the local orientation
    // information to orient strokes.

    Image importance = Image(im.width(), im.height(), im.channels());
    for (int x = 0; x < importance.width(); ++x)
    {
        for (int y = 0; y < importance.height(); ++y)
        {
            importance(x, y, 0) = 1.0f;
            importance(x, y, 1) = 1.0f;
            importance(x, y, 2) = 1.0f;
        }
    }

    Image sharpness = sharpnessMap(im);
    Image out(im.width(), im.height(), im.channels());
    Image angles = computeAngles(im);

    singleScaleOrientedPaint(im, out, angles, importance, texture, strokes, size, noise, 36);
    singleScaleOrientedPaint(im, out, angles, sharpness, texture, strokes, size / 4, noise, 36);

    // out2 is for debugging purposes only
    // Image out2(im.width(), im.height(), im.channels());
    // singleScaleOrientedPaint(im, out2, angles, sharpness, texture, strokes, size / 4, noise, 36);
    // out2.debug_write();
    return out;
}

void lightToDarkPaintHelper(const Image &im,
                            Image &out,
                            const Image &angles,
                            const Image &importance,
                            const Image &texture,
                            int strokes,
                            int size,
                            float noise,
                            int numAngles)
{

    float factor = static_cast<float>(size) / max(texture.width(), texture.height());
    Image scaled_texture = scaleLin(texture, factor);
    std::vector<Image> rotated = rotatedBrushes(scaled_texture, numAngles);

    srand(static_cast<unsigned>(time(0)));

    float average_prob = importance.mean();
    if (average_prob == 0.0f)
    {
        // avoid divide by 0 errors
        // if average_probability = 0, then we always reject.
        return;
    }

    // // Since we rejected a number of samples, we do not splat the required N
    // // strokes. To fix this, multiply N by a normalization factor based on the
    // // average probability of accepting samples.
    int strokes_to_draw = static_cast<int>(static_cast<float>(strokes) / average_prob);
    Image color_image(im.width(), im.height(), im.channels());
    // lumi, x, y
    vector<tuple<float, int, int>> v;
    // [0.3, 0.6, 0.1
    for (int i = 0; i < strokes_to_draw; ++i)
    {
        // generate random x and y coordinates
        int x = rand() % im.width();
        int y = rand() % im.height();

        // if random value < importance sample value, keep. otherwise, skip.
        float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        if (r <= importance(x, y))
        {
            std::vector<float> color;
            for (int c = 0; c < 3; ++c)
            {
                float n = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                float mod = (1.0f - (noise / 2.0f)) + (noise * n);
                color.push_back(im(x, y, c) * mod);
                color_image(x, y, c) = color[c];
            }
            v.push_back(make_tuple(color[0] * 0.3f + color[1] * 0.6f + color[2] * 0.1f, x, y));
        }
    }

    // light to dark means paint strokes with greater luminance first.

    sort(v.begin(), v.end());
    // for (int i = 0; i < v.size(); ++i)

    for (int i = v.size() - 1; i >= 0; --i)
    {
        int x = get<1>(v[i]);
        int y = get<2>(v[i]);
        std::vector<float> q;
        for (int c = 0; c < 3; ++c)
        {
            q.push_back(color_image(x, y, c));
        }
        float angle = angles(x, y);

        Image r = rotate(scaled_texture, angle);

        brush(out, x, y, q, r);
    }
}

Image lightToDarkPaint(const Image &im,
                       const Image &texture,
                       int strokes,
                       int size, float noise)
{
    // Same as painterly but computes and uses the local orientation
    // information to orient strokes.

    Image importance = Image(im.width(), im.height(), im.channels());
    for (int x = 0; x < importance.width(); ++x)
    {
        for (int y = 0; y < importance.height(); ++y)
        {
            importance(x, y, 0) = 1.0f;
            importance(x, y, 1) = 1.0f;
            importance(x, y, 2) = 1.0f;
        }
    }

    Image sharpness = sharpnessMap(im);
    Image out(im.width(), im.height(), im.channels());
    Image angles = computeAngles(im);

    lightToDarkPaintHelper(im, out, angles, importance, texture, strokes, size, noise, 36);
    lightToDarkPaintHelper(im, out, angles, sharpness, texture, strokes, size / 4, noise, 36);

    // out2 is for debugging purposes only
    // Image out2(im.width(), im.height(), im.channels());
    // lightToDarkPaintHelper(im, out2, angles, sharpness, texture, strokes, size / 4, noise, 36);
    // out2.debug_write();
    return out;
}

void darkToLightPaintHelper(const Image &im,
                            Image &out,
                            const Image &angles,
                            const Image &importance,
                            const Image &texture,
                            int strokes,
                            int size,
                            float noise,
                            int numAngles)
{

    float factor = static_cast<float>(size) / max(texture.width(), texture.height());
    Image scaled_texture = scaleLin(texture, factor);
    std::vector<Image> rotated = rotatedBrushes(scaled_texture, numAngles);

    srand(static_cast<unsigned>(time(0)));

    float average_prob = importance.mean();
    if (average_prob == 0.0f)
    {
        // avoid divide by 0 errors
        // if average_probability = 0, then we always reject.
        return;
    }

    // // Since we rejected a number of samples, we do not splat the required N
    // // strokes. To fix this, multiply N by a normalization factor based on the
    // // average probability of accepting samples.
    int strokes_to_draw = static_cast<int>(static_cast<float>(strokes) / average_prob);
    // // cout << "number of strokes to draw:" << strokes_to_draw << endl;

    Image color_image(im.width(), im.height(), im.channels());
    // lumi, x, y
    vector<tuple<float, int, int>> v;
    // [0.3, 0.6, 0.1
    for (int i = 0; i < strokes_to_draw; ++i)
    {
        // generate random x and y coordinates
        int x = rand() % im.width();
        int y = rand() % im.height();

        // if random value < importance sample value, keep. otherwise, skip.
        float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        if (r <= importance(x, y))
        {
            std::vector<float> color;
            for (int c = 0; c < 3; ++c)
            {
                float n = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                float mod = (1.0f - (noise / 2.0f)) + (noise * n);
                color.push_back(im(x, y, c) * mod);
                color_image(x, y, c) = color[c];
            }
            v.push_back(make_tuple(color[0] * 0.3f + color[1] * 0.6f + color[2] * 0.1f, x, y));
        }
    }

    sort(v.begin(), v.end());
    // painting from dark to light means painting those with less luminance first
    for (int i = 0; i < v.size(); ++i)
    {
        int x = get<1>(v[i]);
        int y = get<2>(v[i]);
        std::vector<float> q;
        for (int c = 0; c < 3; ++c)
        {
            q.push_back(color_image(x, y, c));
        }
        float angle = angles(x, y);

        Image r = rotate(scaled_texture, angle);

        brush(out, x, y, q, r);
    }
}

Image darkToLightPaint(const Image &im,
                       const Image &texture,
                       int strokes,
                       int size, float noise)
{
    // Same as painterly but computes and uses the local orientation
    // information to orient strokes.

    Image importance = Image(im.width(), im.height(), im.channels());
    for (int x = 0; x < importance.width(); ++x)
    {
        for (int y = 0; y < importance.height(); ++y)
        {
            importance(x, y, 0) = 1.0f;
            importance(x, y, 1) = 1.0f;
            importance(x, y, 2) = 1.0f;
        }
    }

    Image sharpness = sharpnessMap(im);
    Image out(im.width(), im.height(), im.channels());
    Image angles = computeAngles(im);

    darkToLightPaintHelper(im, out, angles, importance, texture, strokes, size, noise, 36);
    darkToLightPaintHelper(im, out, angles, sharpness, texture, strokes, size / 4, noise, 36);

    // out2 is for debugging purposes only
    // Image out2(im.width(), im.height(), im.channels());
    // darkToLightPaintHelper(im, out2, angles, sharpness, texture, strokes, size / 4, noise, 36);
    // out2.debug_write();
    return out;
}

Image multiScaleOrientedPaint(const Image &im,
                              const Image &texture,
                              int strokes,
                              int size, float noise, int numScales)
{
    Image importance = Image(im.width(), im.height(), im.channels());
    for (int x = 0; x < importance.width(); ++x)
    {
        for (int y = 0; y < importance.height(); ++y)
        {
            importance(x, y, 0) = 1.0f;
            importance(x, y, 1) = 1.0f;
            importance(x, y, 2) = 1.0f;
        }
    }

    Image sharpness = sharpnessMap(im);
    Image out(im.width(), im.height(), im.channels());
    Image angles = computeAngles(im);

    singleScaleOrientedPaint(im, out, angles, importance, texture, strokes, size, noise, 36);

    int scale = 1;
    float truncate = 6.0f;
    float sigma = 2.0f;
    while (scale <= numScales)
    {
        // cout << scale << "," << static_cast<int>(size / scale) << endl;
        sharpness = sharpnessMap(im, sigma, truncate);
        // sharpness.debug_write();
        singleScaleOrientedPaint(im, out, angles, sharpness, texture, strokes, size / (scale * 2), noise, 36);
        scale += 1;
        sigma -= 0.2f;
        truncate -= 0.5f;
    }

    // out2 is for debugging purposes only
    // Image out2(im.width(), im.height(), im.channels());
    // singleScaleOrientedPaint(im, out2, angles, sharpness, texture, strokes, size / 4, noise, 36);
    // out2.debug_write();
    return out;
}
