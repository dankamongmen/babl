/* perform a symmetricality of conversion test on a set of randomized
 * RGBA data */

#include <stdlib.h>
#include <math.h>
#include "babl-internal.h"

#define pixels  1024
int total_length=0;
int total_cost=0;
int total = 0;
int ok = 0;

static double test[pixels * 4];

static void
test_init (void)
{
  int    i;

  for (i = 0; i < pixels * 4; i++)
     test [i] = (double)random () / RAND_MAX;
}

static int qux=0;

static char *utf8_bar[]={" ","·","▁","▂","▃","▄","▅","▆","▇","█"};
//static char *utf8_bar[]=  {"!","▁","▃","▅","▇","█","!","!","!"};
//static char *utf8_bar[]={"·", "█", "▇", "▆", "▅", "▄", "▃", "▂", "▁", };
//static char *utf8_bar[]={" ","1","2","3","4","5","6","7","8"};

static int destination_each (Babl *babl,
                             void *userdata)
{
  Babl *source = userdata;
  Babl *destination = babl;

  if ((qux++) % babl_formats_count () == qux/ babl_formats_count ())
     printf (" ");
  else
    {
      Babl *temp = babl_fish_path (source, destination);

      if (temp)
        {
          printf ("%s", utf8_bar[temp->fish_path.conversions]);
          total_length += temp->fish_path.conversions;
          total_cost   += temp->fish_path.cost;
          ok ++;
          total ++;
        }
      else
        {
          printf (" ");
          total ++;
        }
    }
  return 0;
}

static int source_no=0;

static int source_each (Babl *babl,
                        void *userdata)
{
  babl_format_each (destination_each, babl);
  printf ("──%2i %s\n", source_no++, babl->instance.name);
  return 0;
}

int main (void)
{
  babl_init ();
  test_init ();

  babl_set_extender (babl_extension_quiet_log ());
  babl_format_each (source_each, NULL);
  {
    int i;

      for (i=0;i<babl_formats_count ();i++) printf ("|"); printf ("\n");
      for (i=0;i<babl_formats_count ();i++) if (i/10==0) printf("|"); else printf ("%i", (i/10)%10); printf ("\n");
     /* for (i=0;i<babl_formats_count ();i++) printf ("│"); printf ("\n");
      for (i=0;i<babl_formats_count ();i++) if (i/10==0) printf("│"); else printf ("%i", (i/10)%10); printf ("\n");*/
      for (i=0;i<babl_formats_count ();i++) printf ("%i", (i)%10); printf ("\n");
    }
  printf ("total length: %i\n", total_length);
  printf ("total cost  : %i\n", total_cost);
  /*printf ("ok / total : %i %i %f\n", ok, total, (1.0*ok) / total);
   */

  babl_destroy ();

  return 0;
}