#include <stdio.h>
#include <string.h>
#include <allegro.h>
#include "sound.h"
#define HALBTON 1.059463094

SOUND_CLASS sounds;
const char *sounddir;

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
    KEY_Z,
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
//    KEY_Ö,
    KEY_Y,
    KEY_X,
    KEY_C,
    KEY_V,
    KEY_B,
    KEY_N,
    KEY_M
//    KEY_;,
//    KEY_:
//    KEY_,
};

SOUND_CLASS::SOUND_CLASS()
{
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
};

REGISTER_CLASS::REGISTER_CLASS(const char reg_name[])
{
    total=0;
    name=reg_name;
}

bool REGISTER_CLASS::load(const char filename[], unsigned char taste, int transpose, float tuning)
{
    int soundnr;
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

bool REGISTER_CLASS::load(const char filename[])
{
    int soundnr=sounds.load(filename);
    if (soundnr>=0)
    {
    key[total]= new KEY_CLASS;
    key[total]->tuning=0;
    key[total]->key=tasten1[total];
    key[total]->soundnr=soundnr;
    total++;
    return TRUE;
    }
    else
{
    allegro_message("soundfile %s.wav nicht gefunden", filename);
    return FALSE;
}
}


void REGISTER_CLASS::play(int taste)
{
    int i;
    for(i=0;i<total;i++)
    if(key[i]->key==taste)
    {
       sounds.play(key[i]->soundnr, 128, 128, 1000, 0);
       break;
    }
}

REGISTERS_CLASS::REGISTERS_CLASS()
{
    volume=128;
    total=0;
    act_reg=0;
    transpose=0;
    tuning=0;
    monophon=TRUE;
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

void REGISTERS_CLASS::transpose_set(int value)
{
    transpose=0;
    transpose_plus(value);
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
    registr[act_reg]->load(sound_name);
}

void REGISTERS_CLASS::set_directory(const char *directory_name)
{
    sounddir=directory_name;
}

void REGISTERS_CLASS::play(int taste)
{
    registr[act_reg]->play(taste);
}
