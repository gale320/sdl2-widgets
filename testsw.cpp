/*  
    Demo program for SDL-widgets-2.1
    Copyright 2011-2013 W.Boeke

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this program.
*/
#include <math.h>
#include "sdl-widgets.h"

int col_R,col_G,col_B;
SDL_Color cLightGrey;

TopWin *top_win;
BgrWin *bgwin,
       *rgb_win,
       *ontop_win[3];
Button *hide_show,
       *dummy_but;
RExtButton *extbut[2];
HSlider *sl4,
        *rgb1,*rgb2,*rgb3;
VSlider *sl1,*sl2;
HVSlider *sl3;
Dial *dial;
ExtRButCtrl *ext_rbc;
RButtons *rbw;
TextWin *textw;
SDL_Surface *cross;
HScrollbar *h_scr;
VScrollbar *v_scr,
           *edit_scr;
Lamp *lamp;
CheckBox *rgb_mode;
Message *txt_mes;
DialogWin *dialog;
EditWin *edit_win;
CmdMenu *menu,
        *sel_file;
RenderText *red_font,
           *big_red_font;

namespace spin {
  const int pnt_max=4;
  Point loc[pnt_max]={ Point(-3,5), Point(3,5), Point(3,18), Point(-3,18) },
        mid(120,120),
        aloc[pnt_max]; // actual location
  float radius[pnt_max],
        angle[pnt_max],
        ang=0;
  const float rad2deg=180/M_PI;
  bool run;
  int spin_del=200;
  Button *start_but;
  
  void pointer_draw() {
    Sint16 x[4]={ aloc[0].x, aloc[1].x, aloc[2].x, aloc[3].x },
           y[4]={ aloc[0].y, aloc[1].y, aloc[2].y, aloc[3].y };
  
    filledPolygonColor(bgwin->render,x,y,4,0xffffffff); // pointer
    polygonColor(bgwin->render,x,y,4,0x606060ff);
  
    circleColor(bgwin->render,mid.x,mid.y,2,0xff); // center
  }
  
  void rotate(float ang1) {
    for (int ip=0;ip<pnt_max;++ip) {
      float a=angle[ip]+ang1;
      aloc[ip].x=lrint(cosf(a) * radius[ip] + mid.x);
      aloc[ip].y=lrint(sinf(a) * radius[ip] + mid.y);
    }
  }
  
  void handle_user_event() {
    if (!bgwin) return;
    if (ang>=M_PI) ang-=2*M_PI;
    rotate(ang);
    Rect *r=rp(mid.x-20,mid.y-20,41,41);
    bgwin->clear(r);
    pointer_draw();
    bgwin->blit_upd(r);
    ang+=M_PI / 10.;
  }
  
  int thread_fun(void *arg) {
    while (run) {
      send_uev([](int) { 
        if (!bgwin) return;
        if (ang>=M_PI) ang-=2*M_PI;
        rotate(ang);
        Rect *r=rp(mid.x-20,mid.y-20,41,41);
        bgwin->clear(r);
        pointer_draw();
        bgwin->blit_upd(r);
        ang+=M_PI / 10.;
      },0);
      SDL_Delay(spin_del);
    }
    return 0;
  }
  
  void init() {
    start_but=new Button(bgwin,0,Rect(100,80,32,16),"stop",
      [&](Button *but) {
        run=!run;
        but->label= !run ? "start" : "stop";
        if (run) SDL_CreateThread(thread_fun,"thread_fun", 0);
      });
    for (int i=0;i<pnt_max;++i) {
      float x=loc[i].x,
            y=loc[i].y;
      radius[i]=hypotf(x,y);
      angle[i]=atan2f(x,y);
    }
  }
}

void draw_lissajoux(VSlider *sl) {
  Rect r(sl->tw_area.x+30,sl->tw_area.y+10,50,50);
  boxColor(top_win->renderer,r.x,r.y,r.x+r.w,r.y+r.h,0xffffffff);
  rectangleColor(top_win->renderer,r.x,r.y,r.x+r.w,r.y+r.h,0xff0000ff);
  SDL_SetRenderDrawColor(top_win->renderer,0,0,0,0xff);
  int val=sl->value();
  SDL_Point pnt[r.w+1];
  for (int i=0;i<=r.w;++i) {
    pnt[i].x=r.x+r.w/2+lrint(20*sinf(i * 2 * M_PI / r.w));
    pnt[i].y=r.y+r.h/2+lrint(20*cosf((i+val) * 6 * M_PI / r.w));
  }
  SDL_RenderDrawLines(top_win->renderer,pnt,r.w+1);
}

void disp_bgw(BgrWin *bgw) {
  bgw->clear();
  SDL_BlitSurface(cross,0,bgwin->render.surf,rp(120,60,0,0));
  lamp->draw();
}

static void draw_otwin(BgrWin *bgw) {
  bgw->draw_raised(0,bgw->bgcol,true);
}

void disp_rgb(BgrWin *bgw) {
  bgw->clear();
}

void dial_cmd(Dial* sl,int fire,bool rel) {
  set_text(sl->text,"%d",sl->value());
  spin::spin_del=200/(sl->value()+1);
}

int val2col(HSlider *sl) {
  int col=sl->value()*4;
  if (col>=0x100) return 0xff;
  return col;
}

void bgw_down(BgrWin* bgw,int x,int y,int but) {
  circleColor(bgw->render,x,y,4,0xff);
  bgw->blit_upd(rp(x-4,y-4,9,9));
  SDL_EventState(SDL_MOUSEMOTION,SDL_ENABLE);
}

void bgw_moved(BgrWin*,int x,int y,int but) {
  printf("bgwin: moved %d,%d\n",x,y);
}

void extb_cmd(RExtButton* rb,bool is_act) {
  switch (rb->id) {
    case 'john': printf("john: is_act=%d\n",is_act); break;
    default: if (is_act) alert("dunno you, %s",id2s(rb->id));
  }
}

const char *the_num="9",
           *fnames[20];

void dialog_cmd(const char *val,int id) {
  printf("dialog_cmd: val=%s id=%s\n",val,id2s(id));
  the_num=val;
  dialog->dialog_label("thanks");
}

void edit_cmd(int ctrl_key,int key,int id) {
  printf("key=%d ctrl=%d id=%s\n",key,ctrl_key,id2s(id));
}

void print_path(const char* path,int id) {
  switch (id) {
    case 'file': printf("file: %s\n",path); break;
    case 'dir': printf("dir: %s\n",path); break;
  }
}

void get_sel_fname(const char* path) {
  sel_file->src->label=path;
  sel_file->src->draw_blit_upd();
}

void menu_cmd(RButtons* rb,int nr,int fire) {
  switch (nr) {
    case 0:
      if (!bgwin) { alert("bgwin and rbw gone"); break; }
      printf("rbw-value=%d -> ",rbw->value());
      rbw->set_rbutnr(1,0);
      printf("menu-value=%d rbw-value=%d\n",menu->value(),rbw->value()); fflush(stdout);
      bgwin->set_title("new title",!bgwin->hidden);
      break;
    case 1: {
        alert("Window title:\n  %s",SDL_GetWindowTitle(top_win->window));
        int nr_lines, cursor_ypos, nr_chars, tot_ch;
        bool active;
        edit_win->get_info(&active,&nr_lines,&cursor_ypos,&nr_chars,&tot_ch);
        printf("edit_win: act=%d nr_lines=%d cursor_ypos=%d nr_chars=%d total=%d\n",
                active,nr_lines,cursor_ypos,nr_chars,tot_ch);
      }
      break;
    case 2:
      textw->add_text("hello\n  okay?",true);
      edit_win->insert_line(3,true,"hello okay?");
      edit_win->set_line(0,true,"(empty)");
      break;
    case 3:
      dialog->dialog_label("number:",cPointer);
      dialog->dialog_def(the_num,dialog_cmd,'num');
      break;
    case 4:
      file_chooser(
        [](const char* path) { printf("file: %s\n",path); }
      );
      break;
    case 5:
      working_dir(
        [](const char* path) { printf("dir: %s\n",path); }
      );
      break;
    case 6:
      print_h();
      break;
  }
}

void disp_nr(Render render,int nr,int y_off) {
  draw_ttf->draw_string(render,"num...",Point(4,y_off));
  char buf[20];
  sprintf(buf,"(%s) (id=%d)",the_num,nr);
  draw_ttf->draw_string(render,buf,Point(40,y_off));
}

void handle_keyboard(SDL_Keysym *key,bool down) {
  if (!down) return;
  int k;
  switch (key->sym) {
    case SDLK_d:
    case SDLK_f:
      k=key->sym;
      break;
    default:
      alert("handle_keyb: unexpected key %d",key->sym);
      return;
  }
  printf("key: %c=%d (name=%s) ",k,k,SDL_GetKeyName(key->sym));
  //printf("keystate d:%d f:%d\n",keystate[SDLK_d],keystate[SDLK_f]);
}

#include "icon.xpm"

static const char *cross_xpm[] = {
"10 10 3 1",
" 	c None",
".	c #FF0000",
"X	c #000000",
"          ",
"...       ",
" ...      ",
"  ...   XX",
"   ...XXXX",
"    XXXX  ",
"  XXXX..  ",
"XXXX  ... ",
"XX     ...",
"        .."};

void disp_topw() {
  fill_rect(top_win->render,0,top_win->bgcol);
  txt_mes->draw_label();
  big_red_font->draw_string(top_win->render,"-- bgwin area --",Point(220,50));
  const char* txt="(red font)";
  Rect bg(250,70,red_font->text_width(txt),TDIST);
  fill_rect(top_win->render,&bg,cWhite);
  red_font->draw_string(top_win->render,txt,Point(250,68)); // 2 pixels upwards
  top_win->border(bgwin,2);
}

void handle_resize(int dx,int dy) {
  if (bgwin) bgwin->widen(dx,0);  // widening (only the width)
  sl4->move(dx,0);   // moving
  dial->move(dx,0);
}

static const char* i2s(int i) {
  static char buf[10];
  snprintf(buf,9,"%d",i);
  return buf;
}

void set_icon(SDL_Window* window) {
  SDL_Surface *icon=create_pixmap(icon_xpm);
  SDL_SetWindowIcon(window,icon);
}

void otw_cmd(Button *but) {
  static int otw_cnt=-1;
  otw_cnt=(otw_cnt+1)%3;
  if (!ontop_win[otw_cnt]) {
    int x=0,y=0;
    SDL_Color c=cNop;
    switch (otw_cnt) {
      case 0: x=120; y=200; c=cBlue; break;
      case 1: x=150; y=230; c=cGrey; break;
      case 2: x=100; y=250; c=cForeground; break;
    }
    BgrWin* otw=ontop_win[otw_cnt]=new BgrWin(0,Rect(x,y,80,80),0,draw_otwin,mwin::down,mwin::move,mwin::up,c);
    otw->keep_on_top();
    new Button(otw,Style(0,1),Rect(otw->tw_area.w-20,3,14,14),"X",
      [](Button *b) {
        say("otw_cnt=%d but=%p",otw_cnt,b);
        b->parent->hide();
      }
    );
    otw->draw_blit_recur();
    otw->upd();
  }
  else {
    ontop_win[otw_cnt]->show();
  }
}

int th_fun(void *arg) {
  static int n=0; // output should be: 'th_fun: n=0' 'th_fun: n=1'
  while (n<2) {
    send_uev([=](int) { printf("th_fun: n=%d\n",n); },0);
    ++n;
  }
  return 0;
}

void rgb_cmd(bool rel) {
  SDL_Color new_bgcol={ Uint8(col_R),Uint8(col_G),Uint8(col_B),0 };
  if (rgb_mode->value()) { // draw line
    Rect rect(100,rgb_mode->tw_area.y+5,100,2);
    fill_rect(top_win->render,&rect,new_bgcol);
    top_win->upd(&rect);
  }
  else {
    Rect rect(5,110+rgb_win->tw_area.y,80,10);
    fill_rect(top_win->render,&rect,new_bgcol);
    top_win->upd(&rect);
    if (rel) {
      rgb_win->bgcol=new_bgcol;
      rgb_win->draw_blit_recur(); // all 3 sliders will be drawn
      rgb_win->upd();
    }
  }
}

int main(int ac,char** av) {
  alert("alert: opening top_win...");
  alert_position.set(4,200);

  edit_win=new EditWin(0,Rect(220,230,100,100),"text edit", // parent set to top_win by keep_on_top()
    [](int ctrl_key,int key) {
      printf("key=%d ctrl=%d\n",key,ctrl_key);
    }
  );
  for (int n=0;n<10;++n)
    edit_win->set_line(n,false,"print line %d",n);
  sl1=new VSlider(0,1,Rect(5,40,0,35),5,"vslider1", // parent will be set later
    [](VSlider* sl,int fire,bool rel) {
      set_text(sl->text,"val=%d",sl->value());
      if (sdl_running)
        sl4->set_hsval(sl->value()); // the slave
    }
  );
  sl1->set_vsval(3,1,false);

  top_win=new TopWin("Test SDL-widgets",Rect(100,100,380,500),0,SDL_WINDOW_RESIZABLE,true,disp_topw,set_icon);
  cLightGrey=sdl_col(0xd0d0d0ff);
  edit_win->keep_on_top(); // will cover dummy_but which is instantiated later
  edit_win->bgcol=cLightGrey;
  if (ac>1) alert("no params please");
  handle_kev=handle_keyboard;
  handle_rsev=handle_resize;
  //SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
  const SDL_Color sdl_red={ 0xff,0,0 };
  big_red_font=new RenderText(TTF_OpenFont(def_fontpath,15),sdl_red);
  red_font=new RenderText(draw_ttf->ttf_font,sdl_red);
  hide_show=new Button(top_win,0,Rect(5,10,90,0),"show bgw",
    [](Button *but) {
      if (!bgwin) { alert("bgwin gone fishing"); return; }
      if (bgwin->hidden) {
        bgwin->show(); but->label="hide bgw";
        spin::start_but->cmd(spin::start_but);
      }
      else {
        bgwin->hide(); but->label="show bgw";
        spin::run=false;
      }
    }
  );
  new Button(top_win,0,Rect(5,30,90,0),"delete bgw",
    [](Button *but) {
      if (!bgwin) { alert("bgwin was killed already"); return; }
      bgwin->hide();
      delete bgwin;
      bgwin=0;
    }
  );
  new Button(top_win,0,Rect(5,50,90,0),"set sl4, ext_rbc",
    [](Button *but) {
      if (!bgwin) { alert("slider was killed"); return; }
      sl1->set_vsval(5,1,true);
      ext_rbc->set_rbut(extbut[0],true);
    }
  );
  new Button(top_win,0,Rect(5,70,90,0),"on-top windows",otw_cmd);
  
  menu=new CmdMenu(new Button(top_win,Style(2,1,20),Rect(5,90,50,20),"more ...",[](Button* but) {
    if (!menu->init(100,7,menu_cmd)) return;
    menu->add_mbut("set rbut nr, title");
    menu->add_mbut("window info");
    menu->add_mbut("add text");
    menu->add_mbut(Label(0,disp_nr,3));
    menu->add_mbut("choose file");
    menu->add_mbut("change wdir");
    menu->add_mbut("print widget tree");
  }));
  (new CheckBox(top_win,0,Rect(58,92,0,0),"sticky",0))->d=&menu->sticky;
  sel_file=new CmdMenu(new Button(top_win,Style(4,0),Rect(5,150,100,0),"(none)",
    [](Button *but) { file_chooser(get_sel_fname); },"select file")
  );
  bgwin=new BgrWin(top_win, Rect(200,20,150,145),"bgwin",disp_bgw, bgw_down,bgw_moved,0, cForeground);
  bgwin->hidden=true;
  new Button(bgwin,1,Rect(5,5,0,0),"hide/show slider",
    [](Button* but) {
      if (sl1->hidden || dummy_but->hidden) { sl1->show(); sl2->show(); dummy_but->show(); dial->show(); }
      else { sl1->hide(); sl2->hide(); dummy_but->hide(); dial->hide(); }
    }
  );
  sl1->set_parent(bgwin);
  rbw=new RButtons(bgwin,Style(1,0),Rect(5,94,60,3*TDIST),"rbuttons",false,[](RButtons* rb,int nr,int fire) {
    printf("rbw: maybe_z=%d nr=%d fire=%d\n",rb->maybe_z,nr,fire);
  });
  rbw->add_rbut("1."); rbw->add_rbut("2."); rbw->add_rbut("three");
  rbw->add_rbut(Label([](Render render,int nr,int y_off) {
      draw_ttf->draw_string(render,i2s(nr+1),Point(16,y_off));
      Rect r(26,y_off+5,10,4);
      fill_rect(render,&r,cRed);
      //boxColor(rbw->render,26,y_off+5,36,y_off+9,0xff0000ff);
    })
  );
  rbw->value()=0;
  v_scr=new VScrollbar(bgwin,Style(1,0,5),Rect(70,94,0,3*TDIST),4*TDIST,
    [](VScrollbar *sb) {
      if (rbw->y_off/TDIST!=sb->value/TDIST) {
        rbw->y_off=(sb->value/TDIST)*TDIST;
        rbw->draw_blit_upd();
      }
    }
  );
  ext_rbc=new ExtRButCtrl(Style(1,3),extb_cmd);
  //ext_rbc->maybe_z=false;
  extbut[0]=ext_rbc->add_extrbut(top_win,Rect(110,5,50,0),"john",'john');
  extbut[1]=ext_rbc->add_extrbut(top_win,Rect(110,25,50,0),"pete",'pete');
  new CheckBox(top_win,0,Rect(110,50,0,0),"checkbox1",0);
  new CheckBox(top_win,1,Rect(110,70,0,0),"checkbox2",0);
  textw=new TextWin(top_win,1,Rect(120,110,60,0),4,"text");
  textw->bgcol=cLightGrey;
  textw->add_text("Hello",false);
  cross=create_pixmap(cross_xpm);
  sl2=new VSlider(top_win,Style(0,1),Rect(20,190,0,70),25,"vslider2",
    [](VSlider* sl,int fire,bool rel) {    
          set_text(sl->text,"%d",sl->value());
      if (!bgwin) return;
      if (sl->value()%2) lamp->set_color(0xff0000ff);
      else lamp->set_color(0xff00ff);
      if (top_win->texture) draw_lissajoux(sl); // accellerated?
    }
  );
  lamp=new Lamp(bgwin,Rect(120,40,0,0));
  spin::init();

  sl3=new HVSlider(top_win,0,Rect(120,190,60,60),Int2(4,4),"hvslider",
    [](HVSlider* sl,int fire,bool rel) {
      set_text(sl->text_x,"%d",sl->value().x);
      set_text(sl->text_y,"%d",sl->value().y);
    }
  );

  dummy_but=new Button(top_win,0,Rect(200,260,50,0),"dummy",0);
  static Lazy *lazy_scr;
  sl4=new HSlider(top_win,0,Rect(210,190,60,0),5,"hslider1",
    [](HSlider* sl,int fire,bool rel) {
      printf("%s: rel=%d\n",sl->title_str,rel);
      set_text(sl->text,"%d",sl->value());
    },true);
  sl4->lab_left="0"; sl4->lab_right="5";

  dial=new Dial(top_win,0,Rect(290,190,70,30),7,"dial",dial_cmd);
  dial->set_dialval(2,1,false);
  h_scr=new HScrollbar(top_win,0,Rect(50,285,100,0),400,
    [](HScrollbar* scb) {
      lazy_scr->cmd_done=false;
      if (!lazy_scr->pause) {
        lazy_scr->pause=lazy_scr->cmd_done=true;
        printf("h_scr: val=%d\n",scb->value);
        txt_mes->draw_mes("val=%d, r=%d",scb->value,scb->range);
        lazy_scr->kick();
      }
    }
  );
  lazy_scr=new Lazy([](int) { h_scr->cmd(h_scr); },1000);
  txt_mes=new Message(top_win,2,"scrollbar:",Point(50,h_scr->topleft.y-16));
  new Button(top_win,0,Rect(50,314,20,0),"ok",
    [](Button *but) { dialog->dok(); }
  );
  dialog=new DialogWin(top_win,Rect(72,300,100,0));
  //edit_win=new EditWin(top_win,Rect(220,270,100,60),"text edit",edit_cmd,'edw');
  edit_scr=new VScrollbar(top_win,Style(0,0,5),Rect(322,230,0,100),200,
    [](VScrollbar *sb) { edit_win->set_offset(sb->value); }
  );
  new Button(top_win,Style(0,1),Rect(340,230,14,13),"X",   // hide/show edit_win
    [](Button *but) {
      if (edit_win->hidden) { edit_win->show(); edit_scr->show(); }
      else { edit_win->hide(); edit_scr->hide(); }
    });
  rgb_win=new BgrWin(top_win, Rect(0,340,top_win->tw_area.w,130),"Colors",disp_rgb,0,0,0,sdl_col(0x808080ff));
  rgb1=new HSlider(rgb_win,1,Rect(5,10,top_win->tw_area.w-10,0),64,0,
    [](HSlider* sl,int fire,bool rel) {
      col_R=val2col(sl); set_text(sl->text,"red = %x",col_R);
      rgb_cmd(rel);
    },true);
  rgb2=new HSlider(rgb_win,1,Rect(5,42,top_win->tw_area.w-10,0),64,0,
    [](HSlider* sl,int fire,bool rel) {
      col_G=val2col(sl); set_text(sl->text,"green = %x",col_G);
      rgb_cmd(rel);
    },true);
  rgb3=new HSlider(rgb_win,1,Rect(5,74,top_win->tw_area.w-10,0),64,0,
    [](HSlider* sl,int fire,bool rel) {
      col_B=val2col(sl); set_text(sl->text,"blue = %x",col_B);
      rgb_cmd(rel);
    },true);
  rgb1->value()=rgb2->value()=rgb3->value()=32;
  col_R=col_G=col_B=val2col(rgb1);
  rgb_mode=new CheckBox(top_win,1,Rect(10,480,0,0),"draw line",0);
  
  SDL_CreateThread(th_fun,"th_fun",0);
  get_events();
  return 0;
}
