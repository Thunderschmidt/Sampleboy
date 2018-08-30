#ifndef SND
#define SND
class SOUND_CLASS
{
    private:
    int total;
    const char *name[512];
    SAMPLE *sample[512];
    public:
    int load(const char filename[]);
    void play(int soundnr, int vol, int pan, int freq, int loop);
    SOUND_CLASS();
};

class KEY_CLASS
{
    public:
    unsigned char key;
    int soundnr;
    float tuning;
};

class REGISTER_CLASS
{
    private:
    int total;
    KEY_CLASS *key[128];
    const char *name;
    public:
    bool load(const char filename[], unsigned char taste, int transpose, float tuning);
    bool load(const char filename[]);
    void play(int taste);
    REGISTER_CLASS(const char reg_name[]);
};

class REGISTERS_CLASS
{
    private:
    unsigned char volume;
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
    void transpose_set(int value);
    void init(const char *register_name);
    void load(const char *sound_name);
    void set_directory(const char *directory_name);
    REGISTERS_CLASS();
};
#endif