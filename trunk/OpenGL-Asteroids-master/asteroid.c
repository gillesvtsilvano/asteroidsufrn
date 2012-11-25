#include <stdlib.h>
#include <stdio.h>
#include "asteroid.h"
#include "glwrapper.h"
#include "list.h"

#define ASTEROID_ROTATE_SPEED 10
#define NUM_CIRCLE_POINTS     10
#define ASTEROID_RADIUS       10
#define AST_MOVE_DIST 	      4

#define checkImageWidth 64
#define checkImageHeight 64
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

static GLuint texName;

void makeCheckImage(void)
{
   int i, j, c;
    
   for (i = 0; i < checkImageHeight; i++) {
      for (j = 0; j < checkImageWidth; j++) {
         c = ((((i&0x8)==0)^((j&0x8))==0))*255;
         checkImage[i][j][0] = (GLubyte) c;
         checkImage[i][j][1] = (GLubyte) c;
         checkImage[i][j][2] = (GLubyte) c;
         checkImage[i][j][3] = (GLubyte) 255;
      }
   }
}

static void draw_circle_loop(float radius, int num_points, struct vector2d *coords)
{
    int i;
    float x, y;
    float angle;

    for (i = 0; i < num_points; i++)
    {
        angle = i * (2.0f * M_PI / num_points);
        x = coords->x + cosf(angle) * radius;
        y = coords->y + sinf(angle) * radius;
        glVertex2f(x, y);
	if(x < 0.75)
		x = 0;
	else if(x > 0.75 )
		x = 1.0;
		
	if(y < 0.75)
		y = 0.0;
	else if(y > 0.75 )
		y = -1.0;
		
	glTexCoord2d(x,y);
    }
    glVertex2f(coords->x + radius, coords->y);glTexCoord2d(coords->x + radius, coords->y);	

}

static void draw_circle(float radius, int num_points, struct vector2d *coords)
{
 //glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_POLYGON);
    draw_circle_loop(radius, num_points, coords);
    glEnd();
  // glEnable(GL_TEXTURE_2D);
   //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   //glBindTexture(GL_TEXTURE_2D, texName);
/*glBegin( GL_QUADS );

glTexCoord2d(0.0,0.0); glVertex2d(coords->x +0.0,coords->y + 0.0);
glTexCoord2d(1.0,0.0); glVertex2d(coords->x +100.0,coords->y + 0.0);
glTexCoord2d(1.0,1.0); glVertex2d(coords->x +100.0,coords->y + 100.0);
glTexCoord2d(0.0,1.0); glVertex2d(coords->x +0.0,coords->y + 100.0);
glEnd();*/

}

int check_asteroid_collision(struct vector2d *coords, struct asteroid *asteroid)
{
    return distf(coords, &asteroid->pos.coords) <= asteroid->radius;
}

void move_asteroids(struct asteroid *asteroids)
{
    struct asteroid *tmp;
    int win_h, win_w;

    get_window_size(&win_w, &win_h);
    list_for_each_entry(tmp, &asteroids->list, list)
    {

        update_and_bound_pos(&tmp->pos, AST_MOVE_DIST,
                             0, win_w, 0, win_h);
    }
}

void draw_asteroids(struct asteroid *asteroids)
{
    struct asteroid *tmp;
	
   glShadeModel(GL_FLAT);
   glEnable(GL_TEXTURE_2D);

   makeCheckImage();
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   glGenTextures(1, &texName);
   glBindTexture(GL_TEXTURE_2D, texName);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
     	               GL_NEAREST);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, 
              checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
             checkImage);
   list_for_each_entry(tmp, &asteroids->list, list)
   draw_circle(tmp->radius, NUM_CIRCLE_POINTS, &tmp->pos.coords);
}

void init_asteroid(struct asteroid *asteroid, float x, float y, float angle)
{
    asteroid->pos.coords.x = x;
    asteroid->pos.coords.y = y;
    asteroid->pos.angle = angle;
    asteroid->radius = ASTEROID_RADIUS;
}

void delete_asteroid(struct asteroid *asteroid)
{
    list_del(&asteroid->list);
    free(asteroid);
}

void clear_asteroids(struct asteroid *asteroids)
{
    struct asteroid *asteroid, *n;

    list_for_each_entry_safe(asteroid, n, &asteroids->list, list)
        delete_asteroid(asteroid);
}
