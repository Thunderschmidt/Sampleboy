#include <stdio.h>
#include <math.h>
#include <string.h>
#include <allegro.h>
//#include "sound.h"
#define HALBTON 1.059463094
#define MAXSAMPLES 256
#define MAXMIDIS    128


const char tasten1[] =
{
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_0,
    KEY_Q,
    KEY_W,
    KEY_E,
    KEY_R,
    KEY_T,
    KEY_Y,
    KEY_U,
    KEY_I,
    KEY_O,
    KEY_P,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_COLON,
    KEY_Z,
    KEY_X,
    KEY_C,
    KEY_V,
    KEY_B,
    KEY_N,
    KEY_M,
    KEY_OPENBRACE,
    KEY_CLOSEBRACE,
    KEY_SLASH
};

const char tasten2[] =
{
    KEY_Q,  //a
    KEY_2,
    KEY_W,
    KEY_E,  //c
    KEY_4,
    KEY_R,
    KEY_5,
    KEY_T,
    KEY_Y,
    KEY_7,
    KEY_U,
    KEY_8,
    KEY_I,
    KEY_9,
    KEY_O,   //KEY_P !
    KEY_Z,  //c
    KEY_S,
    KEY_X,
    KEY_D,
    KEY_C,
    KEY_V,
    KEY_G,
    KEY_B,
    KEY_H,
    KEY_N,
    KEY_J,
    KEY_M,
    KEY_OPENBRACE
};


class MIDI_CLASS
{
    private:
    int total;
    const char *name[MAXMIDIS];
    MIDI *midi[MAXMIDIS];
    public:
    int load(const char filename[]);
    void play(int soundnr);
    MIDI_CLASS();
    ~MIDI_CLASS();
    friend void graphic();
};

class SOUND_CLASS
{
    private:
    int total;
    const char *name[MAXSAMPLES];
    SAMPLE *sample[MAXSAMPLES];
    public:
    int load(const char filename[]);
    void play(int soundnr, int vol, int pan, int freq, int loop);
    SOUND_CLASS();
    ~SOUND_CLASS();
    friend void graphic();
};

class KEY_CLASS
{
    public:
    int volume;
    unsigned char key;
    int soundnr;
    float tuning;
    bool ismidi;
    friend void graphic();
};

class REGISTER_CLASS
{
    private:
    int total;
    KEY_CLASS *key[128];
    const char *name;
    public:
    bool load(const char filename[], unsigned char taste, int transpose, float tuning);
    bool load(const char filename[], int vol);
    bool load_midi(const char filename[]);
    void play(int taste, float tuning);

    REGISTER_CLASS(const char reg_name[]);
    friend void graphic();
};

class REGISTERS_CLASS
{
    private:
    int volume;
    int digi_volume, midi_volume;
    int total;
    int act_reg;
    int transpose;
    float tuning;
    bool monophon;
    REGISTER_CLASS *registr[8];
    public:
    void play(int taste);
    void next();
    void last();
    void set(int value);
    void transpose_plus(int value);
    void transpose_reset();
    void init(const char *register_name);
    void load(const char *sound_name);
    void load(const char *sound_name, int vol);
    void set_directory(const char *directory_name);
    void volume_plus(int value);
    void load_midi(const char *midi_name);
    void digi_volume_plus(int value);
    void midi_volume_plus(int value);
    void stop();
    REGISTERS_CLASS();
    friend void graphic();
};

REGISTERS_CLASS registers;
bool keyboard_was_pressed=FALSE;
SOUND_CLASS sounds;
MIDI_CLASS  midis;
const char *sounddir;

MIDI_CLASS::MIDI_CLASS()
{
    total=0;
}

MIDI_CLASS::~MIDI_CLASS()
{
    int i;
    for(i=0;i<total;i++)
    destroy_midi(midi[i]);
    total=0;
}

void MIDI_CLASS::play(int soundnr)
{
   play_midi(midi[soundnr], 0);
}

int MIDI_CLASS::load(const char filename[])
{
     int i;
     for(i=0;i<total;i++)
     {
        if(!strcmp(filename, name[i]))
        {
           return(i);
        }
     }
    char buf[64];
    sprintf(buf, "%s/%s.mid", sounddir, filename);
    midi[total]=load_midi(buf);
    if(midi[total]==NULL)return(-1);
    name[total]=filename;
    total++;
    return(total-1);
}


SOUND_CLASS::SOUND_CLASS()
{
    total=0;
}

SOUND_CLASS::~SOUND_CLASS()
{
    int i;
    for(i=0;i<total;i++)
    destroy_sample(sample[i]);
    total=0;
}
void SOUND_CLASS::play(int soundnr, int vol, int pan, int freq, int loop)
{
    play_sample(sample[soundnr], vol, pan, freq, loop);
}

int SOUND_CLASS::load(const char filename[])
{
     int i;
     for(i=0;i<total;i++)
     {
        if(!strcmp(filename, name[i]))
        {
           return(i);
        }
     }
    char buf[64];
    sprintf(buf, "%s/%s.wav", sounddir, filename);
    sample[total]=load_sample(buf);
    if(sample[total]==NULL)return(-1);
    name[total]=filename;
    total++;
    return(total-1);
}

REGISTER_CLASS::REGISTER_CLASS(const char reg_name[])
{
    total=0;
    name=reg_name;
}

bool REGISTER_CLASS::load(const char filename[], unsigned char taste, int transpose, float tuning)
{
    int soundnr;
    if(total>=40)return FALSE;
    if (soundnr=sounds.load(filename)>0)
    {
    key[total]= new KEY_CLASS;
    key[total]->tuning=tuning;
    key[total]->key=taste;
    key[total]->soundnr=soundnr;
    total++;
    return TRUE;
    }
    else return FALSE;
}

bool REGISTER_CLASS::load(const char filename[], int vol)
{
    int soundnr=sounds.load(filename);
    if (soundnr>=0)
    {
    key[total]= new KEY_CLASS;
    key[total]->tuning=0;
    key[total]->key=tasten1[total];
    key[total]->soundnr=soundnr;
    key[total]->ismidi=FALSE;
    key[total]->volume=vol;
    total++;
    return TRUE;
    }
    else
{
    allegro_message("soundfile %s.wav nicht gefunden", filename);
    return FALSE;
}
}

bool REGISTER_CLASS::load_midi(const char filename[])
{
    int soundnr=midis.load(filename);
    if (soundnr>=0)
    {
    key[total]= new KEY_CLASS;
    key[total]->tuning=0;
    key[total]->key=tasten1[total];
    key[total]->soundnr=soundnr;
    key[total]->ismidi=TRUE;
    total++;
    return TRUE;
    }
    else
    {
       allegro_message("soundfile %s.mid nicht gefunden", filename);
       return FALSE;
    }
}


void REGISTER_CLASS::play(int taste, float tuning)
{
    int i;
    for(i=0;i<total;i++)
    if(key[i]->key==taste)
    {
       if(key[i]->ismidi) midis.play(key[i]->soundnr);
       else sounds.play(key[i]->soundnr, 128+key[i]->volume, 128, tuning*1000, 0);
       break;
    }
}

REGISTERS_CLASS::REGISTERS_CLASS()
{
    midi_volume=255;
    digi_volume=128;
    volume=188;
    total=0;
    act_reg=0;
    transpose=0;
    tuning=1;
    monophon=TRUE;
    set_volume(digi_volume, midi_volume);

}

void REGISTERS_CLASS::next()
{
    act_reg++;
    if (act_reg>=total)act_reg=0;
}

void REGISTERS_CLASS::last()
{
    act_reg--;
    if (act_reg<0)act_reg=total-1;
}

void REGISTERS_CLASS::set(int value)
{
    if((value>=0)&&(value<total)) act_reg=value;
}

void REGISTERS_CLASS::transpose_plus(int value)
{
    int i;
    transpose+=value;
    if(value>0)
    for(i=0;i<value;i++)tuning*=HALBTON;
    else if(value<0)
    for(i=0;i<-value;i++)tuning/=HALBTON;
}

void REGISTERS_CLASS::transpose_reset()
{
    transpose=0;
    tuning=1;
}

void REGISTERS_CLASS::init(const char *register_name)
{
    transpose=0;
    registr[total] = new REGISTER_CLASS(register_name);
    act_reg=total;
    total++;
}

void REGISTERS_CLASS::load(const char *sound_name)
{
    registr[act_reg]->load(sound_name, 0);
}

void REGISTERS_CLASS::load(const char *sound_name, int vol)
{
    registr[act_reg]->load(sound_name, vol);
}

void REGISTERS_CLASS::set_directory(const char *directory_name)
{
    sounddir=directory_name;
}

void REGISTERS_CLASS::play(int taste)
{
    registr[act_reg]->play(taste, tuning);
 //   allegro_message("pups!");
}

void REGISTERS_CLASS::digi_volume_plus(int value)
{
    digi_volume+=value;
    if(digi_volume>255)digi_volume=255;
    else if(digi_volume<0)digi_volume=0;
    set_volume(digi_volume, -1);
}
void REGISTERS_CLASS::midi_volume_plus(int value)
{
    midi_volume+=value;
    if(midi_volume>255)midi_volume=255;
    else if(midi_volume<0)midi_volume=0;
    set_volume(-1, midi_volume);
}

void REGISTERS_CLASS::volume_plus(int value)
{
    midi_volume_plus(value);
    digi_volume_plus(value);
}


void REGISTERS_CLASS::load_midi(const char *midi_name)
{
    registr[act_reg]->load_midi(midi_name);
}

void REGISTERS_CLASS::stop()
{
    stop_midi();
}


PALETTE palette;
BITMAP *image1;
BITMAP *image2;

void graphic()
{
    int i, j;
    int col;
    clear_to_color(screen, palette_color[0]);
    blit(image1, screen, 0, 0, (SCREEN_W-image1->w)/2, 0, image1->w, image1->h);
    blit(image2, screen, 0, 0, (SCREEN_W-image2->w)/2, 265, image2->w, image2->h);
    rect(screen, 240, 319, 560, 365, palette_color[54]);
    rect(screen, 240, 365, 560, 570, palette_color[54]);
    rect(screen,  40, 319, 234, 570, palette_color[54]);
    rect(screen, 566, 319, 760, 570, palette_color[54]);

//    unsigned char buf[32];
    col=palette_color[255];
    textprintf(screen, font,45, 370, col,"Bild<");
    textprintf(screen, font,45, 380, col,"Bild>");
    textprintf(screen, font,45, 390, col,"Pos1");
    textprintf(screen, font,45, 400, col,"Ende");
    textprintf(screen, font,45, 410, col,"Entf");
    textprintf(screen, font,45, 420, col,"up/down");
    textprintf(screen, font,45, 430, col,"left/right");
    textprintf(screen, font,45, 440, col,"Space");
    textprintf(screen, font,45, 450, col,"ESC");

    textprintf(screen, font, 245, 324, col, "F1: ");
    textprintf(screen, font, 245, 334, col, "F2: ");
    textprintf(screen, font, 245, 344, col, "F3: ");
    textprintf(screen, font, 245, 354, col, "F4: ");

    textprintf(screen, font, 405, 324, col, "F5: ");
    textprintf(screen, font, 405, 334, col, "F6: ");
    textprintf(screen, font, 405, 344, col, "F7: ");
    textprintf(screen, font, 405, 354, col, "F8: ");

    textprintf(screen, font, 45, 504, col, "1,2...0:");
    textprintf(screen, font, 45, 514, col, "Q,W...P:");
    textprintf(screen, font, 45, 524, col, "A,S...Ö:");
    textprintf(screen, font, 45, 534, col, "Y,X...-:");

    col=palette_color[159];

    textprintf(screen, font,85, 370, col, ":Halbton aufwaerts");
    textprintf(screen, font,85, 380, col, ":Halbton abwaerts");
    textprintf(screen, font,85, 390, col, ":Oktave aufwaerts");
    textprintf(screen, font,85, 400, col, ":Oktave abwaerts");
    textprintf(screen, font,85, 410, col, ":Transpose Reset");
    textprintf(screen, font,125, 420, col, ":Digi-Volume");
    textprintf(screen, font,125, 430, col, ":Midi-Volume");
    textprintf(screen, font,85, 440, col, ":Sound Stop");
    textprintf(screen, font,85, 450, col, ":verlassen");

    textprintf(screen, font,45, 324, col, "VOLUME-DIGI:");
    textprintf(screen, font,45, 334, col, "VOLUME-MIDI:");
    textprintf(screen, font,45, 344, col, "TRANSPOSE  :");

    textprintf(screen, font,125, 514, col, "Samples");
    textprintf(screen, font,125, 524, col, "abspielen");
    j=registers.registr[registers.act_reg]->total;if(j>20)j=20;
    for(i=0;i<j;i++)
    {
       if(registers.registr[registers.act_reg]->key[i]->ismidi)
       textprintf(screen, font, 245, 370+i*10, col, "%s.mid",
       midis.name[registers.registr[registers.act_reg]->key[i]->soundnr]);
       else
       textprintf(screen, font, 245, 370+i*10, col, "%s.wav",
       sounds.name[registers.registr[registers.act_reg]->key[i]->soundnr]);
    }
    if(registers.registr[registers.act_reg]->total > 20)
    for(i=20;i<registers.registr[registers.act_reg]->total;i++)
    {
       if(registers.registr[registers.act_reg]->key[i]->ismidi)
       textprintf(screen, font, 405, 370+(i-20)*10, col, "%s.mid",
       midis.name[registers.registr[registers.act_reg]->key[i]->soundnr]);
       else
       textprintf(screen, font, 405, 370+(i-20)*10, col, "%s.wav",
       sounds.name[registers.registr[registers.act_reg]->key[i]->soundnr]);
    }




    j=registers.total; if(j>4)j=4;

    for(i=0;i<j;i++)
    {
       if(i==registers.act_reg)col=palette_color[252];
       else col=palette_color[159];
       textprintf(screen, font, 270, 324+i*10, col, "%s", registers.registr[i]->name);
    }
    if(registers.total > 4)
    for(i=4;i<registers.total;i++)
    {
       if(i==registers.act_reg)col=palette_color[252];
       else col=palette_color[159];
       textprintf(screen, font, 430, 324+(i-4)*10, col, "%s", registers.registr[i]->name);
    }

    col=palette_color[252];
    textprintf(screen, font,142, 324, col, "%d", registers.digi_volume);
    textprintf(screen, font,142, 334, col, "%d", registers.midi_volume);
    textprintf(screen, font,142, 344, col, "%d", registers.transpose);

    textprintf(screen, font, 280, 575, palette_color[63], "(c)2005 by Martin Huedepohl");

}

void user_input()
{
    int i, input;
    bool update_screen=FALSE;
    poll_keyboard();
    if(keypressed())
    {
//    if(!keyboard_was_pressed)
//    {
       keyboard_was_pressed=TRUE;
       input=readkey();
       update_screen=TRUE;
       switch(input >> 8)
       {
          case KEY_SPACE:
          registers.stop();
          break;
          case KEY_F1:
          registers.set(0);
          break;     
          case KEY_F2:
          registers.set(1);
          break;     
          case KEY_F3:
          registers.set(2);
          break;     
          case KEY_F4:
          registers.set(3);
          break;
          case KEY_F5:
          registers.set(4);
          break;
          case KEY_F6:
          registers.set(5);
          break;
          case KEY_F7:
          registers.set(6);
          break;
          case KEY_F8:
          registers.set(7);
          break;
          case KEY_PGUP:
          registers.transpose_plus(1);
          break;
          case KEY_PGDN:
          registers.transpose_plus(-1);
          break;
          case KEY_HOME:
          registers.transpose_plus(12);
          break;
          case KEY_END:
          registers.transpose_plus(-12);
          break;
          case KEY_DEL:
          registers.transpose_reset();
          break;
          case KEY_PLUS_PAD:
          registers.volume_plus(8);
          break;
          case KEY_MINUS_PAD:
          registers.volume_plus(-8);
          break;
          case KEY_LEFT:
          registers.midi_volume_plus(-8);
          break;
          case KEY_RIGHT:
          registers.midi_volume_plus(8);
          break;
          case KEY_DOWN:
          registers.digi_volume_plus(-8);
          break;
          case KEY_UP:
          registers.digi_volume_plus(8);
          break;
          default:
          registers.play(input>>8);
          update_screen=FALSE;
          break;
       }
    }

    if(update_screen)graphic();

}

void init_sounds()
{                                             
    registers.set_directory("samples");

    registers.init("WITZGERAEUSCHE");

    registers.load("BOING", 10);
    registers.load("POP");     
    registers.load("JAWHARP", 10);
    registers.load("BOOING");     
    registers.load("FLUMP");     
    registers.load("SQUEEZE");     
    registers.load("KUCKUCK");     
    registers.load("SPLAT", 10);
    registers.load("SQUEEK");     
    registers.load("slide2");     
    registers.load("spring", -10);
    registers.load("whip");
    registers.load("moritz", -10);
    registers.load("HUPE");
    registers.load("SIREN");
    registers.load("KLAXON1");
    registers.load("alarm");
    registers.load("MARTIN1", 30);
    registers.load("MARTIN2", 30);
    registers.set_directory("tusch");
    registers.load("tusch1");
    registers.set_directory("beifall");
    registers.load("CLAP", 127);
    registers.load("ZUGABE", 127);
    registers.load_midi("APPLAUS");
    registers.set_directory("samples");
    registers.load("RAKEXPLO", 127);
     registers.load("ODY-FM04", 64);

    registers.set_directory("pupse");

    registers.init("PUPSE/RUELPSER");

    registers.load("pups01", 45);
    registers.load("pups02");
    registers.load("pups03");
    registers.load("pups04", 65);
    registers.load("pups07");
    registers.load("pups08", 20);
    registers.load("pups09", 100);
    registers.load("pups11");
    registers.load("pups13", -35);
    registers.load("pups14");
    registers.load("pups06", 55);
    registers.load("pups15", -70);
    registers.load("pups05");
    
    registers.set_directory("ruelpser");

    registers.load("ruelps01");
    registers.load("ruelps02");
    registers.load("ruelps03");
    registers.load("ruelps04");
    registers.load("ruelps05");
    registers.load("ruelps06");
    registers.load("ruelps07");

    registers.set_directory("tom");

    registers.init("TOMS");

    registers.load("TOM-1");
    registers.load("TOM0");
    registers.load("TOM1");
    registers.load("TOM2");
    registers.load("TOM3");
    registers.load("TOM4");
    registers.load("TOM5");
    
    registers.set_directory("weapons");

    registers.init("WEAPONS");

    registers.load("TCvDth00");
    registers.load("TMaDth00");
    registers.load("TMaDth01");
    registers.load("Downer");
    registers.load("EXP3");
    registers.load("MACHINEGUN");
    registers.load("PSIBLADE");
    registers.load("SKYNITEFIRE");
    registers.load("SPOOGHIT");
    registers.load("TGoFir00");
    registers.load("wolf_fire");
    registers.load("OALARM");
    registers.load("c4_beep5");
    registers.load("TAdUpd07");
    registers.load("tadUPD04");
    registers.load("tadUpd01");
    registers.load("TAdErr06");
    registers.load("tfbPss02");
    registers.load("tghrdy00");
//    registers.load("");
//    registers.load("");
//    registers.load("");
//    registers.load("");
//    registers.load("");
//    registers.load("");
//    registers.load("");
//    registers.load("");
//    registers.load("");
//    registers.load("");
//    registers.load("");
//    registers.load("");

//
//    registers.init("BEIFALL");
//    registers.set_directory("beifall");
//    registers.load("CLAP", 127);
//    registers.load("ZUGABE", 127);
//    registers.load_midi("APPLAUS");

    registers.init("COUNTERSTRIKE");
    registers.set_directory("counterstrike");
    registers.load("fireinhole");
    registers.load("stormfront");
    registers.load("terwin");


//    registers.init("SONGS");
//    registers.set_directory("schlager");
//    registers.load_midi("beton");
//    registers.load_midi("ozean");
//    registers.load_midi("vanessa");
//    registers.load_midi("warum");
//    registers.load_midi("kohlrou");
//    registers.load_midi("cornelia");
}

int main(void)
{
   allegro_init();
   install_keyboard();
   install_mouse();
   install_timer();
	set_keyboard_rate(0, 0);
   MIDI *midi;
   if (install_sound (DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) < 0)
   {
       printf ("Error initializing sound card");
   }
   init_sounds();
   if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0) != 0)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to set graphic mode\n%s\n", allegro_error);
      return 1;
   }
 //  set_color_conversion(32);

//   palette[0].r=252;   palette[0].g=252;   palette[0].b=252;
image1 = load_bitmap("img/sundm.pcx", palette);
image2 = load_bitmap("img/pupsophon.pcx", NULL);

   set_palette(palette);
   registers.set(0);
   graphic();
   set_volume(128, 255);

   while(!key[KEY_ESC])
   {
      user_input();
   }                    
   return 0;
}

END_OF_MAIN();
