/* vim:expandtab:ts=2 sw=2:
*/
/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2011 Pawel G�ralski
    Copyright 2008 Peter Gordon
    Copyright 2008 Yves Rizoud
    Copyright 2008 Franck Charlet
    Copyright 2007 Adrien Destugues
    Copyright 1996-2001 Sunset Design (Guillaume Dorme & Karl Maritaud)

    Grafx2 is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; version 2
    of the License.

    Grafx2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Grafx2; if not, see <http://www.gnu.org/licenses/>
*/
#include <stdio.h>
#include <string.h>

#if defined(__WIN32__)
    #include <windows.h>
#elif defined(__macosx__) || defined(__FreeBSD__)
    #include <sys/param.h>
    #include <sys/mount.h>
#elif defined (__linux__) || defined(__SYLLABLE__)
    #include <sys/vfs.h>
#elif defined (__HAIKU__)
	#include "haiku.h"
#elif defined (__MINT__)
    #include <mint/sysbind.h>
    #include <mint/osbind.h>
    #include <mint/ostruct.h>
#elif defined(__AROS__)
    #include <sys/mount.h>
#endif

#include "const.h"
#include "struct.h"
#include "global.h"
#include "misc.h"
#include "engine.h"
#include "helpfile.h"
#include "help.h"
#include "sdlscreen.h"
#include "text.h"
#include "keyboard.h"
#include "windows.h"
#include "input.h"
#include "hotkeys.h"
#include "errors.h"
#include "pages.h"
#include "factory.h"

extern char Program_version[]; // generated in pversion.c
extern char SVN_revision[]; // generated in pversion.c

// Recherche un raccourci clavier:
word * Shortcut(word shortcut_number)
{
  if (shortcut_number & 0x100)
    return &(Buttons_Pool[shortcut_number & 0xFF].Left_shortcut[0]);
  if (shortcut_number & 0x200)
    return &(Buttons_Pool[shortcut_number & 0xFF].Right_shortcut[0]);
  return &(Config_Key[shortcut_number & 0xFF][0]);
}

// Nom de la touche actuallement assign�e � un raccourci d'apr�s son num�ro
// de type 0x100+BOUTON_* ou SPECIAL_*
const char * Keyboard_shortcut_value(word shortcut_number)
{
  static char shortcuts_name[80];
  word * pointer = Shortcut(shortcut_number);
  if (pointer == NULL)
    return "(Problem)";
  else
  {
    if (pointer[0] == 0 && pointer[1] == 0)
      return "None";
    if (pointer[0] != 0 && pointer[1] == 0)
      return Key_name(pointer[0]);
    if (pointer[0] == 0 && pointer[1] != 0)
      return Key_name(pointer[1]);
      
    strcpy(shortcuts_name, Key_name(pointer[0]));
    strcat(shortcuts_name, " or ");
    strcat(shortcuts_name, Key_name(pointer[1]));
    return shortcuts_name;
  }
}
void Redefine_control(word *shortcut, int x_pos, int y_pos)
{
  Hide_cursor();
  Print_in_window(x_pos,y_pos,"*PRESS KEY OR BUTTON*",MC_Black,MC_Light);
  Display_cursor();
  while (1)
  {
    Get_input(20);
    if (Key==KEY_ESC)
      return;
    if (Key!=0)
    {
      *shortcut=Key;
      return;
    }
  }    
}

void Window_set_shortcut(int action_id)
{
  short clicked_button;
  short order_index;
  short config_index;
  short redraw_controls=1;
  word * shortcut_ptr=NULL;
  word backup_shortcut[2];
  
  shortcut_ptr=Shortcut(action_id);

  backup_shortcut[0]=shortcut_ptr[0];
  backup_shortcut[1]=shortcut_ptr[1];

  // Recherche dans hotkeys
  order_index=0;
  while (Ordering[order_index]!=action_id)
  {
    order_index++;
    if (order_index>=NB_SHORTCUTS)
    {
      Error(0);
      return;
    }
  }
  /*
  config_index=0;
  while (ConfigKey[config_index].Number!=order_index)
  {
    config_index++;
    if (config_index>=NB_SHORTCUTS)
    {
      Error(0);
      return;
    }
  }
  */
  config_index=order_index; // Comprends pas... �a devrait pas marcher
  
  Open_window(302,131,"Keyboard shortcut");
  Window_set_normal_button(181,111,55,14,"Cancel",0,1,KEY_ESC); // 1
  Window_set_normal_button(241,111,55,14,"OK",0,1,SDLK_RETURN); // 2

  Window_set_normal_button(6,111,111,14,"Reset default",0,1,KEY_NONE); // 3

  // Titre
  Window_rectangle(5,16,292,11,MC_Black);
  Print_in_window(7,18,ConfigKey[config_index].Label,MC_White,MC_Black);

  // Zone de description
  Window_display_frame_in(5,68,292,37);
  Print_in_window(8,70,ConfigKey[config_index].Explanation1,MC_Black,MC_Light);
  Print_in_window(8,78,ConfigKey[config_index].Explanation2,MC_Black,MC_Light);
  Print_in_window(8,86,ConfigKey[config_index].Explanation3,MC_Black,MC_Light);

  // Shortcut 0
  Window_set_normal_button(27,30,177,14,"",0,1,KEY_NONE); // 4
  Window_set_normal_button(209,30,56,14,"Remove",0,1,KEY_NONE); // 5

  // Shortcut 1
  Window_set_normal_button(27,49,177,14,"",0,1,KEY_NONE); // 6
  Window_set_normal_button(209,49,56,14,"Remove",0,1,KEY_NONE); // 7

  Display_cursor();
  do
  {
    if (redraw_controls)
    {
      Hide_cursor();
      Window_rectangle(32,33,21*8,8,MC_Light);
      Print_in_window_limited(32,33,Key_name(shortcut_ptr[0]),21,MC_Black,MC_Light);
      Window_rectangle(32,52,21*8,8,MC_Light);
      Print_in_window_limited(32,52,Key_name(shortcut_ptr[1]),21,MC_Black,MC_Light);
    
      Update_window_area(0,0,302,131);
    
      Display_cursor();
      redraw_controls=0;
    }
    
    clicked_button=Window_clicked_button();

    switch (clicked_button)
    {
      case -1:
      case  0:
      break;
      case 4: // Change 0
        Redefine_control(&shortcut_ptr[0], 32, 33);
        redraw_controls=1;
        break;
      case 6: // Change 1
        Redefine_control(&shortcut_ptr[1], 32, 52);
        redraw_controls=1;
        break;
      case 5: // Remove 0
        shortcut_ptr[0]=0;
        redraw_controls=1;
        break;
      case 7: // Remove 1
        shortcut_ptr[1]=0;
        redraw_controls=1;
        break;
      case 3: // Defaults
        shortcut_ptr[0]=ConfigKey[config_index].Key;
        shortcut_ptr[1]=ConfigKey[config_index].Key2;
        redraw_controls=1;
        break;
      case  1: // Cancel
        shortcut_ptr[0]=backup_shortcut[0];
        shortcut_ptr[1]=backup_shortcut[1];
      case 2: // OK
        // Replace twice by single
        if (shortcut_ptr[0]==shortcut_ptr[1])
          shortcut_ptr[1]=0;
        // Remove all other shortcuts that use same keys
        if (!Config.Allow_multi_shortcuts)
        {
          int n;
          for (n=0; n<2; n++)
          {
            if (shortcut_ptr[n]!=0)
            {
              int i;
              for(i=0; i<NB_SHORTCUTS; i++)
              {
                word * other_shortcut_ptr;
                if (Ordering[i]==action_id)
                  continue;
                other_shortcut_ptr=Shortcut(Ordering[i]);
                if (other_shortcut_ptr[0]==shortcut_ptr[n])
                  other_shortcut_ptr[0]=0;
                if (other_shortcut_ptr[1]==shortcut_ptr[n])
                  other_shortcut_ptr[1]=0;
              }
            }
          }
        }
      default:
        break;
    }
  }
  while ((clicked_button!=1) && (clicked_button!=2) && (Key!=SDLK_RETURN));
  Key=0;
  Close_window();
  Display_cursor();
}

///
/// Browse the complete list of shortcuts and ensure that a key only triggers
/// one of them.
void Remove_duplicate_shortcuts(void)
{
  int action_1;
  // This algorithm favors shortcuts that are last in the list.
  // The idea is that we, coders, append new shortcuts at the end with default
  // values; they take priority as they are new functions.
  for (action_1=NB_SHORTCUTS-1; action_1>0; action_1--)
  { 
    int n;
    word *shortcut_1 = Shortcut(Ordering[action_1]);
    for (n=0; n<2; n++)
    {
      int action_2;
      for (action_2=0; action_2<action_1; action_2++)
      {
        if (shortcut_1[n]!=0)
        {
          int i;
          for(i=0; i<NB_SHORTCUTS; i++)
          {
            word *shortcut_2 = Shortcut(Ordering[action_2]);
            if (shortcut_2[0]==shortcut_1[n])
              shortcut_2[0]=0;
            if (shortcut_2[1]==shortcut_1[n])
              shortcut_2[1]=0;
          }
        }
      }
    }
  }
}

///
/// Print a line with the 'help' (6x8) font.
short Print_help(short x_pos, short y_pos, const char *line, char line_type, short link_position, short link_size)
{
  short  width;             // Largeur physique d'une ligne de texte
  short  x;                   // Indices d'affichage d'un caract�re
  short  y;
  short  x_position;          // Parcours de remplissage du buffer de ligne
  short  char_index; // Parcours des caract�res d'une ligne
  byte * char_pixel;
  short  repeat_menu_x_factor;
  short  repeat_menu_y_factor;
  short  real_x_pos;
  short  real_y_pos;

  real_x_pos=ToWinX(x_pos);
  real_y_pos=ToWinY(y_pos);
  
  // Calcul de la taille
  width=strlen(line);
  // Les lignes de titres prennent plus de place
  if (line_type == 'T' || line_type == '-')
    width = width*2;

  // Pour chaque ligne dans la fen�tre:
  for (y=0;y<8;y++)
  {
    x_position=0;
    // On cr�e une nouvelle ligne � splotcher
    for (char_index=0;char_index<width;char_index++)
    {
      // Recherche du caract�re dans les fontes de l'aide.
      // Ligne titre : Si l'indice est impair on dessine le quart de caract�re
      // qui va a gauche, sinon celui qui va a droite.
      if (line_type=='T')
      {
        if (line[char_index/2]>'_' || line[char_index/2]<' ')
          char_pixel=&(Gfx->Help_font_norm['!'][0][0]); // Caract�re pas g�r�
        else if (char_index & 1)
          char_pixel=&(Gfx->Help_font_t2[(unsigned char)(line[char_index/2])-' '][0][0]);
        else
          char_pixel=&(Gfx->Help_font_t1[(unsigned char)(line[char_index/2])-' '][0][0]);
      }
      else if (line_type=='-')
      {
        if (line[char_index/2]>'_' || line[char_index/2]<' ')
          char_pixel=&(Gfx->Help_font_norm['!'][0][0]); // Caract�re pas g�r�
        else if (char_index & 1)
          char_pixel=&(Gfx->Help_font_t4[(unsigned char)(line[char_index/2])-' '][0][0]);
        else
          char_pixel=&(Gfx->Help_font_t3[(unsigned char)(line[char_index/2])-' '][0][0]);
      }
      else if (line_type=='S')
        char_pixel=&(Gfx->Bold_font[(unsigned char)(line[char_index])][0][0]);
      else if (line_type=='N' || line_type=='K')
        char_pixel=&(Gfx->Help_font_norm[(unsigned char)(line[char_index])][0][0]);
      else
        char_pixel=&(Gfx->Help_font_norm['!'][0][0]); // Un garde-fou en cas de probleme
        
      for (x=0;x<6;x++)
        for (repeat_menu_x_factor=0;repeat_menu_x_factor<Menu_factor_X;repeat_menu_x_factor++)
        {
          byte color = *(char_pixel+x+y*6);
          byte repetition = Pixel_width-1;
          // Surlignement pour liens
          if (line_type=='K' && char_index>=link_position
            && char_index<(link_position+link_size))
          {
            if (color == MC_Light)
              color=MC_White;
            else if (color == MC_Dark)
              color=MC_Light;
            else if (y<7)
              color=MC_Dark;
          }
          Horizontal_line_buffer[x_position++]=color;
          while (repetition--)
            Horizontal_line_buffer[x_position++]=color;
        }
    }
    // On la splotche
    for (repeat_menu_y_factor=0;repeat_menu_y_factor<Menu_factor_Y;repeat_menu_y_factor++)
      Display_line_fast(real_x_pos,real_y_pos++,width*Menu_factor_X*6,Horizontal_line_buffer);
  }
  return width;
}


// -- Menu d'aide -----------------------------------------------------------

void Display_help(void)
{
  short  line_index;     // 0-15 (16 lignes de textes)
  short  start_line=Help_position;
  const short  x_pos=13;
  const short  y_pos=19;
  char   line_type;           // N: Normale, T: Titre, S: Sous-titre
                              // -: Ligne inf�rieur de sous-titre
  const char * line;
  char   buffer[45];          // buffer texte utilis� pour formater les noms de 
                              // raccourcis clavier
  short  link_position=0;     // Position du premier caract�re "variable"
  short  link_size=0;       // Taille de la partie variable
  short width;
  
  for (line_index=0;line_index<16;line_index++)
  {
    // Shortcut au cas ou la section fait moins de 16 lignes
    if (line_index >= Help_section[Current_help_section].Length)
    {
      Window_rectangle (x_pos,
           y_pos + line_index*8,
           44*6,
           // 44 = Nb max de char (+1 pour �viter les plantages en mode X
           // caus�s par une largeur = 0)
           (16 - line_index)*8,
           MC_Black);
      break;
    }
    // On affiche la ligne
    line = Help_section[Current_help_section].Help_table[start_line + line_index].Text;
    line_type = Help_section[Current_help_section].Help_table[start_line + line_index].Line_type;
    // Si c'est une sous-ligne de titre, on utilise le texte de la ligne pr�c�dente
    if (line_type == '-' && (start_line + line_index > 0))
      line = Help_section[Current_help_section].Help_table[start_line + line_index - 1].Text;
    else if (line_type == 'K')
    {
      const char *hyperlink;
      const char * escaped_percent_pos;
      // Determine link position:
      link_position = strstr(line,"%s") - line;
      // Adjust for any escaped %% that would precede it.
      escaped_percent_pos = line;
      do
      {
        escaped_percent_pos = strstr(escaped_percent_pos,"%%");
        if (escaped_percent_pos && escaped_percent_pos - line < link_position)
        {
          link_position--;
          escaped_percent_pos+=2;
        }
      } while (escaped_percent_pos);
      //
      hyperlink=Keyboard_shortcut_value(Help_section[Current_help_section].Help_table[start_line + line_index].Line_parameter);
      link_size=strlen(hyperlink);
      snprintf(buffer, 44, line, hyperlink);
      if (strlen(line)+link_size-2>44)
      {
        buffer[43]=ELLIPSIS_CHARACTER;
        buffer[44]='\0';
      }
      line = buffer;
    }
    
    width=Print_help(x_pos, y_pos+(line_index<<3), line, line_type, link_position, link_size);
    // On efface la fin de la ligne:
    if (width<44)
      Window_rectangle (x_pos+width*6,
           y_pos+(line_index<<3),
           (44-width)*6,
           8,
           MC_Black);
  }
  Update_window_area(x_pos,y_pos,44*6,16*8);
}


void Scroll_help(T_Scroller_button * scroller)
{
  Hide_cursor();
  scroller->Position=Help_position;
  Compute_slider_cursor_length(scroller);
  Window_draw_slider(scroller);
  Display_help();
  Display_cursor();
}


void Button_Help(void)
{
  short btn_number;
  
  // Aide contextuelle
  if (Key!=0)
  {
    btn_number = Button_under_mouse();
    if (btn_number != -1)
    {
      Window_help(btn_number, NULL);
      return;
    }
  }
  Window_help(-1, NULL);
}
// Ouvre l'ecran d'aide. Passer -1 pour la section par d�faut (ou derniere,)
// Ou un nombre de l'enum�ration BUTTON_NUMBERS pour l'aide contextuelle.
void Window_help(int section, const char *sub_section)
{
  short clicked_button;
  short nb_lines;
  T_Scroller_button * scroller;

  if (section!=-1)
  {
    Current_help_section = 4 + section;
    Help_position = 0;
  }
  nb_lines=Help_section[Current_help_section].Length;
  if (section!=-1 && sub_section!=NULL)
  {
    int index=0;
    for (index=0; index<nb_lines; index++)
      if (Help_section[Current_help_section].Help_table[index].Line_type == 'T' &&
        !strcmp(Help_section[Current_help_section].Help_table[index].Text, sub_section))
      {
        Help_position = index;
        break;
      }
  }


  Open_window(310,175,"Help / About...");

  // dessiner de la fen�tre o� va d�filer le texte
  Window_display_frame_in(8,17,274,132);
  Window_rectangle(9, 18, 272, 130, MC_Black);

  Window_set_normal_button(266,153,35,14,"Exit",0,1,KEY_ESC); // 1
  scroller=Window_set_scroller_button(290,18,130,nb_lines,
                                  16,Help_position);   // 2

  Window_set_normal_button(  9,154, 6*8,14,"About"  ,1,1,SDLK_a); // 3

  Window_set_normal_button( 9+6*8+4,154, 8*8,14,"License",1,1,SDLK_l); // 4
  Window_set_normal_button( 9+6*8+4+8*8+4,154, 5*8,14,"Help",1,1,SDLK_h); // 5
  Window_set_normal_button(9+6*8+4+8*8+4+5*8+4,154, 8*8,14,"Credits",1,1,SDLK_c); // 6

  Window_set_special_button(9,18,272,130); // 7

  Display_help();

  Update_window_area(0,0,310,175);

  Display_cursor();

  do
  {
    clicked_button=Window_clicked_button();

    switch (clicked_button)
    {
      case -1:
      case  0:
      case  1:
        break;
      case  7: // Zone de texte
        {
          int line = ((Mouse_Y-Window_pos_Y)/Menu_factor_Y - 18)/8;
          Wait_end_of_click();
          if (line == ((Mouse_Y-Window_pos_Y)/Menu_factor_Y - 18)/8)
          {
            if (Help_position+line<nb_lines)
            {
              switch (Help_section[Current_help_section].Help_table[Help_position+line].Line_type)
              {
                case 'K':
                  Window_set_shortcut(Help_section[Current_help_section].Help_table[Help_position+line].Line_parameter);
                break;
                // Ici on peut g�rer un cas 'lien hypertexte'
                default:
                break;
              }
              Hide_cursor();
              Display_help();
              Display_cursor();
            }
          }
          break;
        }
      default:
        Hide_cursor();
        if (clicked_button>2)
        {
          Current_help_section=clicked_button-3;
          Help_position=0;
          nb_lines=Help_section[Current_help_section].Length;
          scroller->Position=0;
          scroller->Nb_elements=nb_lines;
          Compute_slider_cursor_length(scroller);
          Window_draw_slider(scroller);
        }
        else
          Help_position=Window_attribute2;

        Display_help();
        Display_cursor();
    }


    // Gestion des touches de d�placement dans la liste
    switch (Key)
    {
      case SDLK_UP : // Haut
        if (Help_position>0)
          Help_position--;
        Scroll_help(scroller);
        Key=0;
        break;
      case SDLK_DOWN : // Bas
        if (Help_position<nb_lines-16)
          Help_position++;
        Scroll_help(scroller);
        Key=0;
        break;
      case SDLK_PAGEUP : // PageUp
        if (Help_position>15)
          Help_position-=15;
        else
          Help_position=0;
        Scroll_help(scroller);
        Key=0;
        break;
      case (KEY_MOUSEWHEELUP) : // WheelUp
        if (Help_position>3)
          Help_position-=3;
        else
          Help_position=0;
        Scroll_help(scroller);
        Key=0;
        break;
      case SDLK_PAGEDOWN : // PageDown
        if (nb_lines>16)
        {
          if (Help_position<nb_lines-16-15)
            Help_position+=15;
          else
            Help_position=nb_lines-16;
          Scroll_help(scroller);
          Key=0;
        }
        break;
      case (KEY_MOUSEWHEELDOWN) : // Wheeldown
        if (nb_lines>16)
        {
          if (Help_position<nb_lines-16-3)
            Help_position+=3;
          else
            Help_position=nb_lines-16;
          Scroll_help(scroller);
          Key=0;
        }
        break;
      case SDLK_HOME : // Home
        Help_position=0;
        Scroll_help(scroller);
        Key=0;
        break;
      case SDLK_END : // End
      if (nb_lines>16)
      {
        Help_position=nb_lines-16;
        Scroll_help(scroller);
        Key=0;
      }
        break;
    }
    if (Is_shortcut(Key,0x100+BUTTON_HELP))
      clicked_button=1;
  }
  while ((clicked_button!=1) && (Key!=SDLK_RETURN));

  Key=0;
  Close_window();
  Unselect_button(BUTTON_HELP);
  Display_cursor();
}


#define STATS_TITLE_COLOR  MC_White
#define STATS_DATA_COLOR MC_Light
void Button_Stats(void)
{
  short clicked_button;
  char  buffer[37];
  dword color_usage[256];
  unsigned long long freeRam;
  qword mem_size = 0;
  int y;
#if defined (__MINT__)
  _DISKINFO drvInfo;
  unsigned long STRAM=0,TTRAM=0;
  char helpBuf[64]={0};
#endif
  
  Open_window(310,174,"Statistics");

  // Dessin de la fenetre ou va s'afficher le texte
  Window_display_frame_in(8,17,294,132);
  Window_rectangle(9,18,292,130,MC_Black);

  Window_set_normal_button(120,153,70,14,"OK",0,1,KEY_ESC); // 1

  y=19; // row for first line
  Print_in_window(10,y,"Program version:",STATS_TITLE_COLOR,MC_Black);
  sprintf(buffer,"%s.%s",Program_version, SVN_revision);
  Print_in_window(146,y,buffer,STATS_DATA_COLOR,MC_Black);
  y+=16;
  Print_in_window(10,y,"Build options:",STATS_TITLE_COLOR,MC_Black);
  Print_in_window(146,y,TrueType_is_supported()?"TTF fonts":"no TTF fonts",STATS_DATA_COLOR,MC_Black);
  y+=8;
  Print_in_window(10,y,"Lua version:",STATS_TITLE_COLOR,MC_Black);
  Print_in_window_limited(146,y,Lua_version(),10,STATS_DATA_COLOR,MC_Black);
  y+=16;
  Print_in_window(10,y,"Free memory: ",STATS_TITLE_COLOR,MC_Black);
  y+=8;
  
#if defined (__MINT__)
  // Display free TT/ST RAM
  freeRam=0;

  Atari_Memory_free(&STRAM,&TTRAM);
  freeRam=STRAM+TTRAM;
  buffer[0]='\0';
  
  if(STRAM > (100*1024*1024))
        sprintf(helpBuf,"ST:%u Mb ",(unsigned int)(STRAM/(1024*1024)));
  else if(freeRam > 100*1024)
        sprintf(helpBuf,"ST:%u Kb ",(unsigned int)(STRAM/1024));
  else
        sprintf(helpBuf,"ST:%u b ",(unsigned int)STRAM);

  strncat(buffer,helpBuf,sizeof(char)*37);
  
  if(TTRAM > (100ULL*1024*1024*1024))
        sprintf(helpBuf,"TT:%u Gb",(unsigned int)(TTRAM/(1024*1024*1024)));
  else if(TTRAM > (100*1024*1024))
        sprintf(helpBuf,"TT:%u Mb",(unsigned int)(TTRAM/(1024*1024)));
  else if(freeRam > 100*1024)
        sprintf(helpBuf,"TT:%u Kb",(unsigned int)(TTRAM/1024));
  else
        sprintf(helpBuf,"TT:%u b",(unsigned int)TTRAM);

  strncat(buffer,helpBuf,sizeof(char)*37);

  if(freeRam > (100ULL*1024*1024*1024))
        sprintf(helpBuf,"(%u Gb)",(unsigned int)(freeRam/(1024*1024*1024)));
  else if(freeRam > (100*1024*1024))
        sprintf(helpBuf,"(%u Mb)",(unsigned int)(freeRam/(1024*1024)));
  else if(freeRam > 100*1024)
        sprintf(helpBuf,"(%u Kb)",(unsigned int)(freeRam/1024));
  else
        sprintf(helpBuf,"(%u b)",(unsigned int)freeRam);
   
   strncat(buffer,helpBuf,sizeof(char)*37);
 
   Print_in_window(18,y,buffer,STATS_DATA_COLOR,MC_Black);

#else
  // Display free RAM (generic)
  freeRam = Memory_free();
  
  if(freeRam > (100ULL*1024*1024*1024))
        sprintf(buffer,"%u Gigabytes",(unsigned int)(freeRam/(1024*1024*1024)));
  else if(freeRam > (100*1024*1024))
        sprintf(buffer,"%u Megabytes",(unsigned int)(freeRam/(1024*1024)));
  else if(freeRam > 100*1024)
        sprintf(buffer,"%u Kilobytes",(unsigned int)(freeRam/1024));
  else
        sprintf(buffer,"%u bytes",(unsigned int)freeRam);
  
  Print_in_window(114,y,buffer,STATS_DATA_COLOR,MC_Black);

  #endif
  
  y+=8;
  // Used memory
  Print_in_window(10,y,"Used memory pages: ",STATS_TITLE_COLOR,MC_Black);
  if(Stats_pages_memory > (100LL*1024*1024*1024))
        sprintf(buffer,"%ld (%lld Gb)",Stats_pages_number, Stats_pages_memory/(1024*1024*1024));
  else if(Stats_pages_memory > (100*1024*1024))
        sprintf(buffer,"%ld (%lld Mb)",Stats_pages_number, Stats_pages_memory/(1024*1024));
  else
        sprintf(buffer,"%ld (%lld Kb)",Stats_pages_number, Stats_pages_memory/1024);
  Print_in_window(162,y,buffer,STATS_DATA_COLOR,MC_Black);
  
  y+=8;
#if defined(__WIN32__)
    {
      ULARGE_INTEGER tailleU;
      GetDiskFreeSpaceEx(Main_selector.Directory,&tailleU,NULL,NULL);
      mem_size = tailleU.QuadPart;
    }
#elif defined(__linux__) || defined(__macosx__) || defined(__FreeBSD__) || defined(__SYLLABLE__) || defined(__AROS__)
    {
      struct statfs disk_info;
      statfs(Main_selector.Directory,&disk_info);
      mem_size=(qword) disk_info.f_bfree * (qword) disk_info.f_bsize;
    }
#elif defined(__HAIKU__)
   mem_size = haiku_get_free_space(Main_selector.Directory);
#elif defined (__MINT__)
   mem_size=0;
   Dfree(&drvInfo,0);
   //number of free clusters*sectors per cluster*bytes per sector;
   // reports current drive
   mem_size=drvInfo.b_free*drvInfo.b_clsiz*drvInfo.b_secsiz;
#else
    #define NODISKSPACESUPPORT
    // Free disk space is only for shows. Other platforms can display 0.
    #warning "Missing code for your platform !!! Check and correct please :)"
    mem_size=0;
#endif

  // Display free space
  if (mem_size != 0)
  {
#if defined(__AROS__)
    char *colon = strchr(Main_selector.Directory, ':');
    int len = strlen(Main_selector.Directory);
    if (colon)
    {
      len = (long)colon - (long)Main_selector.Directory;
    }
    if (len > 8) len = 8;
    sprintf(buffer,"Free space on %.*s:",len,Main_selector.Directory);
#else
    sprintf(buffer,"Free space on %c:",Main_selector.Directory[0]);
#endif
    Print_in_window(10,y,buffer,STATS_TITLE_COLOR,MC_Black);

    if(mem_size > (100ULL*1024*1024*1024))
        sprintf(buffer,"%u Gigabytes",(unsigned int)(mem_size/(1024*1024*1024)));
    else if(mem_size > (100*1024*1024))
        sprintf(buffer,"%u Megabytes",(unsigned int)(mem_size/(1024*1024)));
    else if(mem_size > (100*1024))
        sprintf(buffer,"%u Kilobytes",(unsigned int)(mem_size/1024));
    else 
        sprintf(buffer,"%u bytes",(unsigned int)mem_size);
#if defined(__AROS__)
    Print_in_window(192,y,buffer,STATS_DATA_COLOR,MC_Black);
#else
    Print_in_window(146,y,buffer,STATS_DATA_COLOR,MC_Black);
#endif
  } else {
	#ifndef NODISKSPACESUPPORT
	  Print_in_window(10,y,"Disk full!",STATS_TITLE_COLOR,MC_Black);
	#endif
	#undef NODISKSPACESUPPORT
  }
  
  y+=16;
  // Affichage des informations sur l'image
  Print_in_window(10,y,"Picture info.:",STATS_TITLE_COLOR,MC_Black);
  y+=8;
  
  // Affichage des dimensions de l'image
  Print_in_window(18,y,"Dimensions :",STATS_TITLE_COLOR,MC_Black);
  sprintf(buffer,"%dx%d",Main_image_width,Main_image_height);
  Print_in_window(122,y,buffer,STATS_DATA_COLOR,MC_Black);
  y+=8;
  
  // Affichage du nombre de couleur utilis�
  Print_in_window(18,y,"Colors used:",STATS_TITLE_COLOR,MC_Black);
  memset(color_usage,0,sizeof(color_usage));
  sprintf(buffer,"%d",Count_used_colors(color_usage));
  Print_in_window(122,y,buffer,STATS_DATA_COLOR,MC_Black);
  y+=16;
  
  // Affichage des dimensions de l'�cran
  Print_in_window(10,y,"Resolution:",STATS_TITLE_COLOR,MC_Black);
  sprintf(buffer,"%dx%d",Screen_width,Screen_height);
  Print_in_window(106,y,buffer,STATS_DATA_COLOR,MC_Black);
  
  Update_window_area(0,0,310,174);

  Display_cursor();

  do
  {
    clicked_button=Window_clicked_button();
    if (Is_shortcut(Key,0x200+BUTTON_HELP))
      clicked_button=1;
  }
  while ( (clicked_button!=1) && (Key!=SDLK_RETURN) );

  if(Key==SDLK_RETURN)Key=0;

  Close_window();
  Unselect_button(BUTTON_HELP);
  Display_cursor();
}

