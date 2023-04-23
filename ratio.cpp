#include <stdio.h>


int main(int argc, char const* const* argv)
{
  if( argc != 3 )
  {
    fprintf(stderr, "Computes image sizes to preserve the ratio.\n", argv[0]);
    fprintf(stderr, "Usage: %s <screen_WidthxHeight> <image_WidthxHeight>\n", argv[0]);
    fprintf(stderr, "for example: %s 1280x1024 980x657\n", argv[0]);
    return -1;
  }

  int sw = 0, sh = 0;
  sscanf(argv[1], "%dx%d", &sw, &sh);
  fprintf(stdout, "screen dimensions: \t%dx%d\n", sw, sh);

  int iw = 0, ih = 0;
  sscanf(argv[2], "%dx%d", &iw, &ih);
  fprintf(stdout, "image dimensions: \t%dx%d\n", iw, ih);

  if( sw <= 0 || sh <= 0 || iw <= 0 || ih <= 0 )
  {
    fprintf(stderr, "all dimensions must be positive integers\n");
    return -2;
  }

  double ratio = (double)sw / (double)sh;

  int niw = int((double)ih * ratio + 0.5);
  int nih = ih;

  if( niw > iw )
  {
    niw = iw;
    nih = int((double)iw / ratio + 0.5);
  }

  fprintf(stdout, "new image dimensions: \t%dx%d\n", niw, nih);

  return 0;
}
