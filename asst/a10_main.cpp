#include <iostream>
#include "a10.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>

using namespace std;

void testBrush()
{
  Image black(1000, 1000, 3);
  Image long_brush("./Input/longBrush.png");
  Image long_brush2("./Input/longBrush2.png");
  std::vector<float> red = {255, 0, 0};
  std::vector<float> green = {0, 255, 0};
  std::vector<float> blue = {0, 0, 255};

  brush(black, 450, 100, red, long_brush2);
  brush(black, 200, 100, green, long_brush);
  brush(black, 400, 400, blue, long_brush2);

  black.write("./Output/opaque_brush.png");

  // now, test with half opaque brush to make sure linear blend
  Image long_brush_half_opaque(long_brush.width(), long_brush.height(), long_brush.channels());
  for (int x = 0; x < long_brush_half_opaque.width(); ++x)
  {
    for (int y = 0; y < long_brush_half_opaque.height(); ++y)
    {
      for (int c = 0; c < long_brush_half_opaque.channels(); ++c)
      {
        long_brush_half_opaque(x, y, c) = 0.5f * long_brush(x, y, c);
      }
    }
  }
  Image black_two(1000, 1000, 3);

  brush(black_two, 450, 100, red, long_brush2);
  brush(black_two, 200, 100, green, long_brush);
  brush(black_two, 700, 400, blue, long_brush2);
  black_two.write("./Output/half_opaque_brush.png");
};

void testSingleScalePaint()
{
  Image archie("./Input/archie.png");
  Image ville("./Input/villeperdue.png");
  Image brush("./Input/brush.png");
  Image long_brush2("./Input/longBrush2.png");

  Image importance_a(archie.width(), archie.height(), archie.channels());
  Image importance_v(ville.width(), ville.height(), ville.channels());

  Image out_a(archie.width(), archie.height(), archie.channels());
  Image out_v(ville.width(), ville.height(), ville.channels());

  for (int x = 0; x < importance_a.width(); ++x)
  {
    for (int y = 0; y < importance_a.height(); ++y)
    {
      importance_a(x, y, 0) = 1.0f;
      importance_a(x, y, 1) = 1.0f;
      importance_a(x, y, 2) = 1.0f;
    }
  }

  for (int x = 0; x < importance_v.width(); ++x)
  {
    for (int y = 0; y < importance_v.height(); ++y)
    {
      importance_v(x, y, 0) = 1.0f;
      importance_v(x, y, 1) = 1.0f;
      importance_v(x, y, 2) = 1.0f;
    }
  }

  singleScalePaint(archie, out_a, importance_a, brush, 30, 10000);
  out_a.write("./Output/single_scale_archie.png");

  singleScalePaint(ville, out_v, importance_v, long_brush2, 30, 10000);
  out_v.write("./Output/single_scale_ville.png");

  // // now, test importance map
  // Image out_a2(archie.width(), archie.height(), archie.channels());
  // Image out_v2(ville.width(), ville.height(), ville.channels());

  // for (int x = 0; x < importance_a.width() / 2; ++x)
  // {
  //   for (int y = 0; y < importance_a.height() / 2; ++y)
  //   {
  //     importance_a(x, y, 0) = 1.0f;
  //     importance_a(x, y, 1) = 1.0f;
  //     importance_a(x, y, 2) = 1.0f;
  //   }
  // }

  // for (int x = 0; x < importance_v.width() / 2; ++x)
  // {
  //   for (int y = 0; y < importance_v.height() / 2; ++y)
  //   {
  //     importance_v(x, y, 0) = 1.0f;
  //     importance_v(x, y, 1) = 1.0f;
  //     importance_v(x, y, 2) = 1.0f;
  //   }
  // }

  // singleScalePaint(archie, out_a2, importance_a, brush, 30, 10000);
  // out_a2.write("./Output/single_scale_archie_importance.png");

  // singleScalePaint(ville, out_v2, importance_v, long_brush2, 30, 10000);
  // out_v2.write("./Output/single_scale_ville_importance.png");
};

void testPainterly()
{
  Image archie("./Input/archie.png");
  Image train("./Input/train.png");
  Image taipei("./Input/taipei.png");
  Image hae("./Input/hae.png");

  Image ville("./Input/villeperdue.png");
  Image brush("./Input/brush.png");
  Image long_brush("./Input/longBrush.png");
  Image long_brush2("./Input/longBrush2.png");

  Image painterly_archie = painterly(archie, brush);
  painterly_archie.write("./Output/painterly_archie.png");

  Image painterly_archie_long = painterly(archie, long_brush);
  painterly_archie_long.write("./Output/painterly_archie_long.png");

  Image painterly_archie_long2 = painterly(archie, long_brush2);
  painterly_archie_long2.write("./Output/painterly_archie_long2.png");

  Image painterly_ville = painterly(ville, brush);
  painterly_ville.write("./Output/painterly_ville.png");

  Image painterly_ville_long = painterly(ville, long_brush);
  painterly_ville_long.write("./Output/painterly_ville_long.png");

  Image painterly_ville_long2 = painterly(ville, long_brush2);
  painterly_ville_long2.write("./Output/painterly_ville_long2.png");

  Image painterly_taipei = painterly(taipei, brush);
  painterly_taipei.write("./Output/painterly_taipei.png");

  Image painterly_train = painterly(train, brush);
  painterly_train.write("./Output/painterly_train.png");

  Image painterly_hae = painterly(hae, brush);
  painterly_hae.write("./Output/painterly_hae.png");
};

void testComputeAngles()
{
  Image archie("./Input/archie.png");
  Image ville("./Input/villeperdue.png");

  Image brush("./Input/brush.png");
  Image long_brush("./Input/longBrush.png");
  Image long_brush2("./Input/longBrush2.png");

  Image angles_archie = computeAngles(archie);
  angles_archie.write("./Output/angles_archie.png");

  Image angles_ville = computeAngles(ville);
  angles_ville.write("./Output/angles_ville.png");
};

void testComputeAngles_simple()
{
  Image d("./Input/diagonal.png");
  Image d2("./Input/diagonal2.png");

  Image brush("./Input/brush.png");
  Image long_brush("./Input/longBrush.png");
  Image long_brush2("./Input/longBrush2.png");

  Image angles_d = computeAngles(d);
  angles_d.write("./Output/angles_d.png");

  Image angles_d2 = computeAngles(d2);
  angles_d2.write("./Output/angles_d2.png");
}

void testRotatedImages()
{
  Image archie("./Input/archie.png");
  Image ville("./Input/villeperdue.png");

  Image brush("./Input/brush.png");
  Image long_brush("./Input/longBrush.png");
  Image long_brush2("./Input/longBrush2.png");

  std::vector<Image> rotated = rotatedBrushes(brush);

  for (int i = 0; i < rotated.size(); ++i)
  {
    ostringstream fname;
    fname << "./Output/rotated_brush_";
    fname << setfill('0') << setw(2);
    fname << i;
    fname << ".png";
    rotated[i].write(fname.str());
  }

  // std::vector<Image> rotated = rotatedBrushes(brush);

  // for (int i = 0; i < rotated.size(); ++i)
  // {
  //   ostringstream fname;
  //   fname << "./Output/rotated_brush_";
  //   fname << setfill('0') << setw(2);
  //   fname << i;
  //   fname << ".png";
  //   rotated[i].write(fname.str());
  // }
};

void testSharpnessMap()
{
  Image archie("./Input/archie.png");
  Image ville("./Input/villeperdue.png");

  Image s_archie = sharpnessMap(archie);
  s_archie.write("./Output/sharpness_archie.png");

  Image s_ville = sharpnessMap(ville);
  s_ville.write("./Output/sharpness_ville.png");
}
void testSSOPHelper(const Image &i, const std::string &name, int numStrokes, int size)
{
  Image brush("./Input/brush.png");
  Image long_brush("./Input/longBrush.png");
  Image long_brush2("./Input/longBrush2.png");

  Image out(i.width(), i.height(), i.channels());
  Image angles = computeAngles(i);
  Image importance(i.width(), i.height(), i.channels());

  for (int x = 0; x < i.width(); ++x)
  {
    for (int y = 0; y < i.height(); ++y)
    {
      importance(x, y, 0) = 1.0f;
      importance(x, y, 1) = 1.0f;
      importance(x, y, 2) = 1.0f;
    }
  }

  singleScaleOrientedPaint(i, out, angles, importance, brush, numStrokes, size, 0.2, 36);
  ostringstream fname;
  fname << "./Output/SSOP_";
  fname << name;
  fname << ".png";
  out.write(fname.str());
}

void testSingleScaleOrientedPaint_simple()
{
  Image h("./Input/horizontal.png");
  Image v("./Input/vertical.png");
  Image v2("./Input/vertical2.png");
  Image d("./Input/diagonal.png");
  Image d2("./Input/diagonal2.png");
  testSSOPHelper(h, "h", 10000, 20);
  testSSOPHelper(v, "v", 10000, 20);
  testSSOPHelper(v2, "v2", 10000, 20);
  testSSOPHelper(d, "d", 10000, 20);
  testSSOPHelper(d2, "d2", 10000, 20);
}

void testSingleScaleOrientedPaint()
{
  Image archie("./Input/archie.png");
  Image ville("./Input/villeperdue.png");

  Image brush("./Input/brush.png");
  Image long_brush("./Input/longBrush.png");
  Image long_brush2("./Input/longBrush2.png");

  testSSOPHelper(archie, "archie", 20000, 20);
  testSSOPHelper(ville, "ville", 20000, 20);
}

void testOrientedPaint()
{
  Image archie("./Input/archie.png");
  Image ville("./Input/villeperdue.png");
  Image boston("./Input/boston.png");
  Image bp("./Input/bp-1-2.png");
  Image bp4("./Input/bp-4.png");
  Image castle("./Input/castle.png");
  Image china("./Input/china.png");
  Image wedding("./Input/DSC_0579.png");

  Image train("./Input/train.png");
  Image taipei("./Input/taipei.png");
  Image hae("./Input/hae.png");

  Image brush("./Input/brush.png");
  Image long_brush("./Input/longBrush.png");
  Image long_brush2("./Input/longBrush2.png");

  Image out_ville = orientedPaint(ville, brush, 10000, 50, 0.3f);
  out_ville.write("./Output/orientedPaint_ville.png");

  Image out_china = orientedPaint(china, brush, 10000, 50, 0.3f);
  out_china.write("./Output/orientedPaint_china.png");

  Image out_wedding = orientedPaint(wedding, brush, 10000, 50, 0.3f);
  out_wedding.write("./Output/orientedPaint_wedding.png");

  Image out_train = orientedPaint(train, brush, 10000, 50, 0.3f);
  out_train.write("./Output/orientedPaint_train.png");

  Image out_taipei = orientedPaint(taipei, brush, 10000, 50, 0.3f);
  out_taipei.write("./Output/orientedPaint_taipei.png");

  Image out_hae = orientedPaint(hae, brush, 10000, 50, 0.3f);
  out_hae.write("./Output/orientedPaint_hae.png");
}

void testLightToDarkPaint()
{
  Image archie("./Input/archie.png");
  Image ville("./Input/villeperdue.png");
  Image boston("./Input/boston.png");
  Image bp("./Input/bp-1-2.png");
  Image bp4("./Input/bp-4.png");
  Image castle("./Input/castle.png");
  Image china("./Input/china.png");
  Image wedding("./Input/DSC_0579.png");
  Image canyon("./Input/DSC_8268.png");
  Image street("./Input/DSC_8394.png");

  Image circle("./Input/round.png");
  Image stata("./Input/stata.png");

  Image train("./Input/train.png");
  Image taipei("./Input/taipei.png");
  Image hae("./Input/hae.png");

  Image brush("./Input/brush.png");
  Image long_brush("./Input/longBrush.png");
  Image long_brush2("./Input/longBrush2.png");

  Image out_ville = lightToDarkPaint(ville, brush, 10000, 50, 0.3f);
  out_ville.write("./Output/lightToDark_ville.png");

  Image out_archie = lightToDarkPaint(archie, brush, 10000, 50, 0.3f);
  out_archie.write("./Output/lightToDark_archie.png");

  // Image out_castle = lightToDarkPaint(castle, brush, 10000, 50, 0.3f);
  // out_castle.write("./Output/lightToDark_castle.png");

  Image out_china = lightToDarkPaint(china, brush, 10000, 50, 0.3f);
  out_china.write("./Output/lightToDark_china.png");

  // Image out_canyon = lightToDarkPaint(canyon, brush, 10000, 50, 0.3f);
  // out_canyon.write("./Output/lightToDark_canyon.png");

  // Image out_street = lightToDarkPaint(street, brush, 10000, 50, 0.3f);
  // out_street.write("./Output/lightToDark_street.png");

  Image out_wedding = lightToDarkPaint(wedding, brush, 10000, 50, 0.3f);
  out_wedding.write("./Output/lightToDark_wedding.png");

  Image out_train = lightToDarkPaint(train, brush, 10000, 50, 0.3f);
  out_train.write("./Output/lightToDark_train.png");

  Image out_taipei = lightToDarkPaint(taipei, brush, 10000, 50, 0.3f);
  out_taipei.write("./Output/lightToDark_taipei.png");

  Image out_hae = lightToDarkPaint(hae, brush, 10000, 50, 0.3f);
  out_hae.write("./Output/lightToDark_hae.png");

}

void testDarkToLightPaint()
{
  Image archie("./Input/archie.png");
  Image ville("./Input/villeperdue.png");
  Image castle("./Input/castle.png");
  Image china("./Input/china.png");
  Image wedding("./Input/DSC_0579.png");
  // Image canyon("./Input/DSC_8268.png");
  Image street("./Input/DSC_8394.png");

  Image train("./Input/train.png");
  Image taipei("./Input/taipei.png");
  Image hae("./Input/hae.png");
  Image brush("./Input/brush.png");
  Image long_brush("./Input/longBrush.png");
  Image long_brush2("./Input/longBrush2.png");

  Image out_ville = darkToLightPaint(ville, brush, 10000, 50, 0.3f);
  out_ville.write("./Output/darkToLight_ville.png");

  Image out_archie = darkToLightPaint(archie, brush, 10000, 50, 0.3f);
  out_archie.write("./Output/darkToLight_archie.png");

  // Image out_castle = darkToLightPaint(castle, brush, 10000, 50, 0.3f);
  // out_castle.write("./Output/darkToLight_castle.png");

  // Image out_china = darkToLightPaint(china, brush, 10000, 50, 0.3f);
  // out_china.write("./Output/darkToLight_china.png");

  // Image out_canyon = darkToLightPaint(canyon, brush, 10000, 50, 0.3f);
  // out_canyon.write("./Output/darkToLight_canyon.png");

  // Image out_street = darkToLightPaint(street, brush, 10000, 50, 0.3f);
  // out_street.write("./Output/darkToLight_street.png");

  Image out_wedding = darkToLightPaint(wedding, brush, 10000, 50, 0.3f);
  out_wedding.write("./Output/darkToLight_wedding.png");

  Image out_train = darkToLightPaint(train, brush, 10000, 50, 0.3f);
  out_train.write("./Output/darkToLight_train.png");

  Image out_taipei = darkToLightPaint(taipei, brush, 10000, 50, 0.3f);
  out_taipei.write("./Output/darkToLight_taipei.png");

  Image out_hae = darkToLightPaint(hae, brush, 10000, 50, 0.3f);
  out_hae.write("./Output/darkToLight_hae.png");

}

void testMultiScaleOrientedPaint()
{
  Image archie("./Input/archie.png");
  Image ville("./Input/villeperdue.png");
  Image castle("./Input/castle.png");
  Image china("./Input/china.png");
  Image wedding("./Input/DSC_0579.png");
  Image canyon("./Input/DSC_8268.png");
  Image street("./Input/DSC_8394.png");
  Image train("./Input/train.png");
  Image taipei("./Input/taipei.png");
  Image hae("./Input/hae.png");

  Image brush("./Input/brush.png");
  Image long_brush("./Input/longBrush.png");
  Image long_brush2("./Input/longBrush2.png");

  Image out_ville = multiScaleOrientedPaint(ville, brush, 10000, 100, 0.3f, 4);
  out_ville.write("./Output/multiscale_ville.png");

  Image out_train = multiScaleOrientedPaint(train, brush, 10000, 50, 0.3f, 4);
  out_train.write("./Output/multiscale_train.png");

  Image out_taipei = multiScaleOrientedPaint(taipei, brush, 10000, 50, 0.3f, 4);
  out_taipei.write("./Output/multiscale_taipei.png");

  Image out_hae = darkToLightPaint(hae, brush, 10000, 50, 0.3f);
  out_hae.write("./Output/multiscale_hae.png");
}
int main()
{
  // Test your intermediate functions
  // testBrush();
  testSingleScalePaint();
  testPainterly();

  // testComputeAngles();
  // testRotatedImages();

  testSingleScaleOrientedPaint();
  testOrientedPaint();

  testSingleScaleOrientedPaint();

  testDarkToLightPaint();
  testLightToDarkPaint();

  testMultiScaleOrientedPaint();

  testSharpnessMap();
  return EXIT_SUCCESS;
}
