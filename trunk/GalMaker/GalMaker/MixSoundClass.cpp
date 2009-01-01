#include "MixSoundClass.h"

//*******************************
//class BaseMixSound

int BaseMixSound::MixNUM = 0;

BaseMixSound::BaseMixSound()
{
	if ( MixNUM == 0 ){
		if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ){
			std::cerr << "Mix_Open ERROR" << std::endl;
			exit(-1);
		}
	}
	MixNUM++;
}

BaseMixSound::~BaseMixSound()
{
	MixNUM--;
	if ( MixNUM == 0 ){
		Mix_CloseAudio();
	}
}

SDL_RWops* BaseMixSound::GetFileFromPack(const std::string &fileInPack, const std::string &packName)
{
	unzFile zip;
	unz_file_info info;
	Uint8 *buffer;

	zip = unzOpen( packName.c_str() );
	unzLocateFile( zip, fileInPack.c_str(), 0 );
	unzGetCurrentFileInfo( zip, &info, NULL, 0, NULL, 0, NULL, 0 );
	unzOpenCurrentFile( zip );
	buffer = (Uint8*)malloc(info.uncompressed_size);
	unzReadCurrentFile( zip, buffer, info.uncompressed_size );

	return SDL_RWFromMem(buffer, info.uncompressed_size);
}

//*******************************



//*******************************
//class EffectSound

EffectSound::EffectSound(const std::string& sound_fileName, const std::string& packName)
{
	if(unzOpen( packName.c_str() )!=NULL)
	{
		SDL_RWops* rw=GetFileFromPack(sound_fileName, packName);
		sound = Mix_LoadWAV_RW(rw,0);
	}
	else
	{
		std::string dir="./sound/";
		dir=dir+sound_fileName;
		sound = Mix_LoadWAV(dir.c_str());
	}
}

EffectSound::~EffectSound()
{
	Mix_FreeChunk(sound);
}

void EffectSound::play() const
{
	 if( Mix_PlayChannel(-1, sound, 0) == -1 ){
		 std::cerr << "Mix_PlayChannel() ERROR" << std::endl;
	 }
}

//*******************************



//*******************************
//class MusicSound

MusicSound::MusicSound(const std::string& music_fileName, const std::string& packName)
{

	if(unzOpen( packName.c_str() )!=NULL)
	{
		SDL_RWops* rw=GetFileFromPack(music_fileName, packName);
		music = Mix_LoadMUS_RW(rw);
	}
	else
	{
		std::string dir="./music/";
		dir=dir+music_fileName;
		music =Mix_LoadMUS(dir.c_str());
	}

}

MusicSound::~MusicSound()
{
	Mix_FreeMusic(music);
}

void MusicSound::play() const
{
	if( Mix_PlayingMusic() == 0 ){
		if( Mix_PlayMusic( music, -1 ) == -1 ){
			std::cerr << "Mix_PlayMusic() ERROR" << std::endl;
		}
	} else {
		if( Mix_PausedMusic() == 1) {
			Mix_ResumeMusic();
		}
		else {
			Mix_PauseMusic();
		}
	}
}

void MusicSound::stop() const
{
	Mix_HaltMusic();
}

//***************************************

