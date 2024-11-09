/*
 * tclsndfile.c
 *
 *      Copyright (C) Danilo Chang 2016-2018
 *
 ********************************************************************/

/*
 * For C++ compilers, use extern "C"
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <tcl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sndfile.h>

extern DLLEXPORT int    Sndfile_Init(Tcl_Interp * interp);

/*
 * end block for C++
 */

#ifdef __cplusplus
}
#endif

typedef struct SndFileData SndFileData;

struct SndFileData {
  SNDFILE *sndfile;
  Tcl_Interp *interp;
  int mode;
  SF_INFO sfinfo;
  int buffersize;
  int buff_init;
  short *short_block;
  int *int_block;
  float *float_block;
  double *double_block;
};

TCL_DECLARE_MUTEX(myMutex);


static int SndObjCmd(void *cd, Tcl_Interp *interp, int objc,Tcl_Obj *const*objv){
  SndFileData *pSnd = (SndFileData *) cd;
  int choice;
  int rc = TCL_OK;

  static const char *SND_strs[] = {
    "buffersize",
    "read_short",
    "read_int",
    "read_float",
    "read_double",
    "write_short",
    "write_int",
    "write_float",
    "write_double",
    "seek",
    "get_string",
    "set_string",
    "close", 
    0
  };

  enum SND_enum {
    SND_BUFFERSIZE,
    SND_READ_SHORT,
    SND_READ_INT,
    SND_READ_FLOAT,
    SND_READ_DOUBLE,
    SND_WRITE_SHORT,
    SND_WRITE_INT,
    SND_WRITE_FLOAT,
    SND_WRITE_DOUBLE,
    SND_SEEK,
    SND_GET_STRING,
    SND_SET_STRING,
    SND_CLOSE,
  };

  if( objc < 2 ){
    Tcl_WrongNumArgs(interp, 1, objv, "SUBCOMMAND ...");
    return TCL_ERROR;
  }

  if( Tcl_GetIndexFromObj(interp, objv[1], SND_strs, "option", 0, &choice) ){
    return TCL_ERROR;
  }

  switch( (enum SND_enum)choice ){

    case SND_BUFFERSIZE: {
      int buffersize = 0;

      if( objc != 3 ){
        Tcl_WrongNumArgs(interp, 2, objv, "size");
        return TCL_ERROR;
      }

      if(Tcl_GetIntFromObj(interp, objv[2], &buffersize) != TCL_OK) {
         return TCL_ERROR;
      }

      if(buffersize <= 0) {
         Tcl_AppendResult(interp, "Error: buffersize needs > 0", (char*)0);
         return TCL_ERROR;
      }

      Tcl_MutexLock(&myMutex);
      if(pSnd->buff_init == 0) {
        pSnd->buffersize = buffersize;
        pSnd->buff_init = 1;
      }
      Tcl_MutexUnlock(&myMutex);
      break;
    }

    case SND_READ_SHORT: {
      Tcl_Obj *return_obj = NULL;
      long read_count = 0;
      long second_count = pSnd->sfinfo.samplerate * pSnd->sfinfo.channels;

      if( objc != 2 ){
        Tcl_WrongNumArgs(interp, 2, objv, 0);
        return TCL_ERROR;
      }

      // It is still 0 -> setup the value
      if(pSnd->buffersize == 0) {
         Tcl_MutexLock(&myMutex);
         pSnd->buffersize = second_count;
         pSnd->buff_init = 1;
         Tcl_MutexUnlock(&myMutex);
      }

      if(pSnd->short_block == NULL) {
         pSnd->short_block = (short *) malloc (pSnd->buffersize * sizeof(short));
         if( pSnd->short_block == 0 ){
           Tcl_SetResult(interp, (char *)"malloc failed", TCL_STATIC);
           return TCL_ERROR;
         }
      }

      read_count = sf_read_short(pSnd->sndfile, pSnd->short_block, pSnd->buffersize);

      if(read_count <= 0) {
         return TCL_ERROR;
      } else {
         return_obj = Tcl_NewByteArrayObj((unsigned char *) pSnd->short_block, read_count * sizeof(short));
         Tcl_SetObjResult(interp, return_obj);
      }

      break;
    }

    case SND_READ_INT: {
      Tcl_Obj *return_obj = NULL;
      long read_count = 0;
      long second_count = pSnd->sfinfo.samplerate * pSnd->sfinfo.channels;

      if( objc != 2 ){
        Tcl_WrongNumArgs(interp, 2, objv, 0);
        return TCL_ERROR;
      }

      // It is still 0 -> setup the value
      if(pSnd->buffersize == 0) {
         Tcl_MutexLock(&myMutex);
         pSnd->buffersize = second_count;
         pSnd->buff_init = 1;
         Tcl_MutexUnlock(&myMutex);
      }

      if(pSnd->int_block == NULL) {
         pSnd->int_block = (int *) malloc (pSnd->buffersize * sizeof(int));
         if( pSnd->int_block == 0 ){
           Tcl_SetResult(interp, (char *)"malloc failed", TCL_STATIC);
           return TCL_ERROR;
         }
      }

      read_count = sf_read_int(pSnd->sndfile, pSnd->int_block, pSnd->buffersize);

      if(read_count <= 0) {
         return TCL_ERROR;
      } else {
         return_obj = Tcl_NewByteArrayObj((unsigned char *) pSnd->int_block, read_count * sizeof(int));
         Tcl_SetObjResult(interp, return_obj);
      }

      break;
    }

    case SND_READ_FLOAT: {
      Tcl_Obj *return_obj = NULL;
      long read_count = 0;
      long second_count = pSnd->sfinfo.samplerate * pSnd->sfinfo.channels;

      if( objc != 2 ){
        Tcl_WrongNumArgs(interp, 2, objv, 0);
        return TCL_ERROR;
      }

      // It is still 0 -> setup the value
      if(pSnd->buffersize == 0) {
         Tcl_MutexLock(&myMutex);
         pSnd->buffersize = second_count;
         pSnd->buff_init = 1;
         Tcl_MutexUnlock(&myMutex);
      }

      if(pSnd->float_block == NULL) {
         pSnd->float_block = (float *) malloc (pSnd->buffersize * sizeof(float));
         if( pSnd->float_block == 0 ){
           Tcl_SetResult(interp, (char *)"malloc failed", TCL_STATIC);
           return TCL_ERROR;
         }
      }

      read_count = sf_read_float(pSnd->sndfile, pSnd->float_block, pSnd->buffersize);

      if(read_count <= 0) {
         return TCL_ERROR;
      } else {
         return_obj = Tcl_NewByteArrayObj((unsigned char *) pSnd->float_block, read_count * sizeof(float));
         Tcl_SetObjResult(interp, return_obj);
      }

      break;
    }

    case SND_READ_DOUBLE: {
      Tcl_Obj *return_obj = NULL;
      long read_count = 0;
      long second_count = pSnd->sfinfo.samplerate * pSnd->sfinfo.channels;

      if( objc != 2 ){
        Tcl_WrongNumArgs(interp, 2, objv, 0);
        return TCL_ERROR;
      }

      // It is still 0 -> setup the value
      if(pSnd->buffersize == 0) {
         Tcl_MutexLock(&myMutex);
         pSnd->buffersize = second_count;
         pSnd->buff_init = 1;
         Tcl_MutexUnlock(&myMutex);
      }

      if(pSnd->double_block == NULL) {
         pSnd->double_block = (double *) malloc (pSnd->buffersize * sizeof(double));
         if( pSnd->double_block == 0 ){
           Tcl_SetResult(interp, (char *)"malloc failed", TCL_STATIC);
           return TCL_ERROR;
         }
      }

      read_count = sf_read_double(pSnd->sndfile, pSnd->double_block, pSnd->buffersize);

      if(read_count <= 0) {
         return TCL_ERROR;
      } else {
         return_obj = Tcl_NewByteArrayObj((unsigned char *) pSnd->double_block, read_count * sizeof(double));
         Tcl_SetObjResult(interp, return_obj);
      }

      break;
    }

    case SND_WRITE_SHORT: {
      Tcl_Obj *return_obj = NULL;
      unsigned char *zData = NULL;
      Tcl_Size len;
      sf_count_t count;

      if( objc != 3 ){
        Tcl_WrongNumArgs(interp, 2, objv,
          "byte_array"
        );
        return TCL_ERROR;
      }

      zData = Tcl_GetByteArrayFromObj(objv[2], &len);
      if( !zData || len < 1 ){
          return TCL_ERROR;
      }

      count = sf_write_short(pSnd->sndfile, (short *) zData, len/sizeof(short));
      return_obj = Tcl_NewIntObj((sf_count_t) count);
      Tcl_SetObjResult(interp, return_obj);

      break;
    }

    case SND_WRITE_INT: {
      Tcl_Obj *return_obj = NULL;
      unsigned char *zData = NULL;
      Tcl_Size len;
      sf_count_t count;

      if( objc != 3 ){
        Tcl_WrongNumArgs(interp, 2, objv,
          "byte_array"
        );
        return TCL_ERROR;
      }

      zData = Tcl_GetByteArrayFromObj(objv[2], &len);
      if( !zData || len < 1 ){
          return TCL_ERROR;
      }

      count = sf_write_int(pSnd->sndfile, (int *) zData, len/sizeof(int));
      return_obj = Tcl_NewIntObj((sf_count_t) count);
      Tcl_SetObjResult(interp, return_obj);

      break;
    }

    case SND_WRITE_FLOAT: {
      Tcl_Obj *return_obj = NULL;
      unsigned char *zData = NULL;
      Tcl_Size len;
      sf_count_t count;

      if( objc != 3 ){
        Tcl_WrongNumArgs(interp, 2, objv,
          "byte_array"
        );
        return TCL_ERROR;
      }

      zData = Tcl_GetByteArrayFromObj(objv[2], &len);
      if( !zData || len < 1 ){
          return TCL_ERROR;
      }

      count = sf_write_float(pSnd->sndfile, (float *) zData, len/sizeof(float));
      return_obj = Tcl_NewIntObj((sf_count_t) count);
      Tcl_SetObjResult(interp, return_obj);

      break;
    }

    case SND_WRITE_DOUBLE: {
      Tcl_Obj *return_obj = NULL;
      unsigned char *zData = NULL;
      Tcl_Size len;
      sf_count_t count;

      if( objc != 3 ){
        Tcl_WrongNumArgs(interp, 2, objv,
          "byte_array"
        );
        return TCL_ERROR;
      }

      zData = Tcl_GetByteArrayFromObj(objv[2], &len);
      if( !zData || len < 1 ){
          return TCL_ERROR;
      }

      count = sf_write_double(pSnd->sndfile, (double *) zData, len/sizeof(double));
      return_obj = Tcl_NewIntObj((sf_count_t) count);
      Tcl_SetObjResult(interp, return_obj);

      break;
    }

    case SND_SEEK: {
      Tcl_Obj *return_obj = NULL;
      int location = 0;
      const char *pWhence = NULL;
      int whence = SEEK_CUR;
      Tcl_Size len;
      sf_count_t count;

      if( objc != 4 ){
        Tcl_WrongNumArgs(interp, 2, objv,
          "location whence"
        );
        return TCL_ERROR;
      }

      if(pSnd->sfinfo.seekable) {
        if(Tcl_GetIntFromObj(interp, objv[2], &location) != TCL_OK) {
            return TCL_ERROR;
        }

        pWhence = Tcl_GetStringFromObj(objv[3], &len);
        if( !whence || len < 1 ){
            return TCL_ERROR;
        }

        //SEEK_SET  - set to the start of the audio data plus offset
        //SEEK_CUR  - set to its current location plus offset 
        //SEEK_END  - set to the end of the data plus offset
        if( strcmp(pWhence, "SET")==0 ){
          whence = SEEK_SET;
        } else if( strcmp(pWhence, "CUR")==0 ){
          whence = SEEK_CUR;
        } else if( strcmp(pWhence, "END")==0 ){
          whence = SEEK_END;
        }

        count = sf_seek(pSnd->sndfile, (sf_count_t) location, whence);

        return_obj = Tcl_NewIntObj((sf_count_t) count);
        Tcl_SetObjResult(interp, return_obj);
      } else {
          Tcl_SetResult(interp, (char *)"Not seekable", TCL_STATIC);
          return TCL_ERROR;
      }
      break;
    }

    case SND_GET_STRING: {
      Tcl_Obj *return_obj = NULL;
      const char *pType = NULL;
      const char *pResult = NULL;
      int str_type = 0;
      Tcl_Size len = 0;

      if( objc != 3 ){
        Tcl_WrongNumArgs(interp, 2, objv,
          "str_type"
        );
        return TCL_ERROR;
      }

      pType = Tcl_GetStringFromObj(objv[2], &len);
      if( !pType || len < 1 ){
        return TCL_ERROR;
      }

      if(pSnd->mode != SFM_READ && pSnd->mode != SFM_RDWR) {
        return TCL_ERROR;
      }

      if( strcmp(pType, "SF_STR_TITLE")==0 ){
        str_type = SF_STR_TITLE;
      } else if( strcmp(pType, "SF_STR_COPYRIGHT")==0 ){
        str_type = SF_STR_COPYRIGHT;
      } else if( strcmp(pType, "SF_STR_SOFTWARE")==0 ){
        str_type = SF_STR_SOFTWARE;
      } else if( strcmp(pType, "SF_STR_ARTIST")==0 ){
        str_type = SF_STR_ARTIST;
      } else if( strcmp(pType, "SF_STR_COMMENT")==0 ){
        str_type = SF_STR_COMMENT;
      } else if( strcmp(pType, "SF_STR_DATE")==0 ){
        str_type = SF_STR_DATE;
      } else if( strcmp(pType, "SF_STR_ALBUM")==0 ){
        str_type = SF_STR_ALBUM;
      } else if( strcmp(pType, "SF_STR_LICENSE")==0 ){
        str_type = SF_STR_LICENSE;
      } else if( strcmp(pType, "SF_STR_TRACKNUMBER")==0 ){
        str_type = SF_STR_TRACKNUMBER;
      } else if( strcmp(pType, "SF_STR_GENRE")==0 ){
        str_type = SF_STR_GENRE;
      } else {
        Tcl_AppendResult(interp, "unknown option: ", pType, (char*)0);
        return TCL_ERROR;
      }

      pResult = sf_get_string(pSnd->sndfile, str_type);

      if(pResult) {
         return_obj = Tcl_NewStringObj(pResult, -1);
      } else {
         return_obj = Tcl_NewStringObj("", 0);
      }

      Tcl_SetObjResult(interp, return_obj);

      break;
    }

    case SND_SET_STRING: {
      Tcl_Obj *return_obj = NULL;
      const char *pType = NULL;
      const char *pString = NULL;
      int str_type = 0;
      Tcl_Size len = 0;
      int result = 0;

      if( objc != 4 ){
        Tcl_WrongNumArgs(interp, 2, objv,
          "str_type string"
        );
        return TCL_ERROR;
      }

      pType = Tcl_GetStringFromObj(objv[2], &len);
      if( !pType || len < 1 ){
        return TCL_ERROR;
      }

      pString = Tcl_GetStringFromObj(objv[3], &len);
      if( !pString || len < 1 ){
        return TCL_ERROR;
      }

      if(pSnd->mode != SFM_WRITE && pSnd->mode != SFM_RDWR) {
        return TCL_ERROR;
      }

      if( strcmp(pType, "SF_STR_TITLE")==0 ){
        str_type = SF_STR_TITLE;
      } else if( strcmp(pType, "SF_STR_COPYRIGHT")==0 ){
        str_type = SF_STR_COPYRIGHT;
      } else if( strcmp(pType, "SF_STR_SOFTWARE")==0 ){
        str_type = SF_STR_SOFTWARE;
      } else if( strcmp(pType, "SF_STR_ARTIST")==0 ){
        str_type = SF_STR_ARTIST;
      } else if( strcmp(pType, "SF_STR_COMMENT")==0 ){
        str_type = SF_STR_COMMENT;
      } else if( strcmp(pType, "SF_STR_DATE")==0 ){
        str_type = SF_STR_DATE;
      } else if( strcmp(pType, "SF_STR_ALBUM")==0 ){
        str_type = SF_STR_ALBUM;
      } else if( strcmp(pType, "SF_STR_LICENSE")==0 ){
        str_type = SF_STR_LICENSE;
      } else if( strcmp(pType, "SF_STR_TRACKNUMBER")==0 ){
        str_type = SF_STR_TRACKNUMBER;
      } else if( strcmp(pType, "SF_STR_GENRE")==0 ){
        str_type = SF_STR_GENRE;
      } else {
        Tcl_AppendResult(interp, "unknown option: ", pType, (char*)0);
        return TCL_ERROR;
      }

      result = sf_set_string(pSnd->sndfile, str_type, pString);

      /*
       * The sf_set_string() function sets the string data.
       * It returns zero on success and non-zero on error.
       */
      return_obj = Tcl_NewIntObj(result);
      Tcl_SetObjResult(interp, return_obj);

      break;
    }

    case SND_CLOSE: {
      int result = 0;
      Tcl_Obj *return_obj = NULL;

      if( objc != 2 ){
        Tcl_WrongNumArgs(interp, 2, objv, 0);
        return TCL_ERROR;
      }

      result = sf_close(pSnd->sndfile);

      if(pSnd->short_block) free(pSnd->short_block);
      if(pSnd->int_block) free(pSnd->int_block);
      if(pSnd->float_block) free(pSnd->float_block);
      if(pSnd->double_block) free(pSnd->double_block);
      Tcl_Free((char *)pSnd);
      pSnd = NULL;

      Tcl_DeleteCommand(interp, Tcl_GetStringFromObj(objv[0], 0));

      return_obj = Tcl_NewIntObj(result);
      Tcl_SetObjResult(interp, return_obj);
      break;
    }

  } /* End of the SWITCH statement */

  return rc;
}


static int SndMain(void *cd, Tcl_Interp *interp, int objc,Tcl_Obj *const*objv){
  SndFileData *p;
  const char *zArg;
  int i = 0;
  const char *zFile = NULL;
  const char *zMode = NULL;
  char *fileformat = NULL;
  char *encoding = NULL;
  int samplerate = 44100;
  int channels = 2;
  Tcl_DString translatedFilename;
  int buffersize = 0;
  Tcl_Obj *pResultStr = NULL;
  Tcl_Size len;

  if( objc<4 || (objc&1)!=0 ){
    Tcl_WrongNumArgs(interp, 1, objv,
      "HANDLE path mode ?-buffersize size? ?-rate samplerate? ?-channels channels? ?-fileformat format? ?-encoding encoding_type? "
    );
    return TCL_ERROR;
  }

  p = (SndFileData *)Tcl_Alloc( sizeof(*p) );
  if( p==0 ){
    Tcl_SetResult(interp, (char *)"malloc failed", TCL_STATIC);
    return TCL_ERROR;
  }

  memset(p, 0, sizeof(*p));
  p->interp = interp;

  zFile = Tcl_GetStringFromObj(objv[2], &len);
  if( !zFile || len < 1 ){
    Tcl_Free((char *)p);
    return TCL_ERROR;
  }

  zMode = Tcl_GetStringFromObj(objv[3], &len);
  if( !zMode || len < 1 ){
    Tcl_Free((char *)p);

    Tcl_AppendResult(interp, "Error: correct mode is READ, WRITE and RDWR", (char*)0);
    return TCL_ERROR;
  }

  //SFM_READ    - read only mode
  //SFM_WRITE   - write only mode
  //SFM_RDWR    - read/write mode
  if( strcmp(zMode, "READ")==0 ){
    p->mode = SFM_READ;
  } else if( strcmp(zMode, "WRITE")==0 ){
    p->mode = SFM_WRITE;
  } else if( strcmp(zMode, "RDWR")==0 ){
    p->mode = SFM_RDWR;
  } else {
    Tcl_Free((char *)p);

    Tcl_AppendResult(interp, "Error: correct mode is READ, WRITE and RDWR", (char*)0);
    return TCL_ERROR;
  }

  for(i=4; i+1<objc; i+=2){
    zArg = Tcl_GetStringFromObj(objv[i], 0);

    if( strcmp(zArg, "-buffersize")==0 ){
      if(Tcl_GetIntFromObj(interp, objv[i+1], &buffersize) != TCL_OK) {
         Tcl_Free((char *)p);
         return TCL_ERROR;
      }

      if(buffersize <= 0) {
         Tcl_Free((char *)p);
         Tcl_AppendResult(interp, "Error: buffersize needs > 0", (char*)0);
         return TCL_ERROR;
      }

      Tcl_MutexLock(&myMutex);
      p->buffersize = buffersize;
      p->buff_init = 1;
      Tcl_MutexUnlock(&myMutex);
    } else if( strcmp(zArg, "-rate")==0 ){
      if(Tcl_GetIntFromObj(interp, objv[i+1], &samplerate) != TCL_OK) {
         Tcl_Free((char *)p);
         return TCL_ERROR;
      }

      if(samplerate <= 0) {
         Tcl_Free((char *)p);
         Tcl_AppendResult(interp, "Error: samplerate needs > 0", (char*)0);
         return TCL_ERROR;
      }
    } else if( strcmp(zArg, "-channels")==0 ){
      if(Tcl_GetIntFromObj(interp, objv[i+1], &channels) != TCL_OK) {
         Tcl_Free((char *)p);
         return TCL_ERROR;
      }
      
      if(channels <= 0) {
         Tcl_Free((char *)p);
         Tcl_AppendResult(interp, "Error: channels needs > 0", (char*)0);
         return TCL_ERROR;
      }
    } else if( strcmp(zArg, "-fileformat")==0 ){
      fileformat = Tcl_GetStringFromObj(objv[i+1], &len);
      if( !fileformat || len < 1 ){
         Tcl_Free((char *)p);
         return TCL_ERROR;
      }
    } else if( strcmp(zArg, "-encoding")==0 ){
      encoding = Tcl_GetStringFromObj(objv[i+1], &len);
      if( !encoding || len < 1 ){
         Tcl_Free((char *)p);
         return TCL_ERROR;
      }
    }else{
      Tcl_Free((char *)p);

      Tcl_AppendResult(interp, "unknown option: ", zArg, (char*)0);
      return TCL_ERROR;
    }
  }

  if(p->mode != SFM_READ) {
    if(!fileformat || !encoding) {
      Tcl_Free((char *)p);

      Tcl_AppendResult(interp, "Error: fileformat and encoding need specify a value", (char*)0);
      return TCL_ERROR;
    }

    p->sfinfo.samplerate = samplerate;
    p->sfinfo.channels = channels;  

    if(strcmp(fileformat, "wav")==0) {
        p->sfinfo.format |= SF_FORMAT_WAV;
    } else if(strcmp(fileformat, "aiff")==0) {
        p->sfinfo.format |= SF_FORMAT_AIFF;
    } else if(strcmp(fileformat, "au")==0) {
        p->sfinfo.format |= SF_FORMAT_AU;
    } else if(strcmp(fileformat, "raw")==0) {
        p->sfinfo.format |= SF_FORMAT_RAW;
    } else if(strcmp(fileformat, "paf")==0) {
        p->sfinfo.format |= SF_FORMAT_PAF;
    } else if(strcmp(fileformat, "svx")==0) {
        p->sfinfo.format |= SF_FORMAT_SVX;
    } else if(strcmp(fileformat, "nist")==0) {
        p->sfinfo.format |= SF_FORMAT_NIST;
    } else if(strcmp(fileformat, "voc")==0) {
        p->sfinfo.format |= SF_FORMAT_VOC;
    } else if(strcmp(fileformat, "ircam")==0) {
        p->sfinfo.format |= SF_FORMAT_IRCAM;
    } else if(strcmp(fileformat, "w64")==0) {
        p->sfinfo.format |= SF_FORMAT_W64;
    } else if(strcmp(fileformat, "mat4")==0) {
        p->sfinfo.format |= SF_FORMAT_MAT4;
    } else if(strcmp(fileformat, "mat5")==0) {
        p->sfinfo.format |= SF_FORMAT_MAT5;
    } else if(strcmp(fileformat, "pvf")==0) {
        p->sfinfo.format |= SF_FORMAT_PVF;
    } else if(strcmp(fileformat, "xi")==0) {
        p->sfinfo.format |= SF_FORMAT_XI;
    } else if(strcmp(fileformat, "htk")==0) {
        p->sfinfo.format |= SF_FORMAT_HTK;
    } else if(strcmp(fileformat, "sds")==0) {
        p->sfinfo.format |= SF_FORMAT_SDS;
    } else if(strcmp(fileformat, "avr")==0) {
        p->sfinfo.format |= SF_FORMAT_AVR;
    } else if(strcmp(fileformat, "wavex")==0) {
        p->sfinfo.format |= SF_FORMAT_WAVEX;
    } else if(strcmp(fileformat, "sd2")==0) {
        p->sfinfo.format |= SF_FORMAT_SD2;
    } else if(strcmp(fileformat, "flac")==0) {
        p->sfinfo.format |= SF_FORMAT_FLAC;
    } else if(strcmp(fileformat, "caf")==0) {
        p->sfinfo.format |= SF_FORMAT_CAF;
    } else if(strcmp(fileformat, "wve")==0) {
        p->sfinfo.format |= SF_FORMAT_WVE;
    } else if(strcmp(fileformat, "ogg")==0) {
        p->sfinfo.format |= SF_FORMAT_OGG;
    } else if(strcmp(fileformat, "mpc2k")==0) {
        p->sfinfo.format |= SF_FORMAT_MPC2K;
    } else if(strcmp(fileformat, "rf64")==0) {
        p->sfinfo.format |= SF_FORMAT_RF64;
    } else {
       Tcl_Free((char *)p);

       Tcl_AppendResult(interp, "fileformat unknown option", (char*)0);
       return TCL_ERROR;
    }

    if(strcmp(encoding, "pcm_16")==0) {
        p->sfinfo.format |= SF_FORMAT_PCM_16;
    } else if(strcmp(encoding, "pcm_24")==0) {
        p->sfinfo.format |= SF_FORMAT_PCM_24;
    } else if(strcmp(encoding, "pcm_32")==0) {
        p->sfinfo.format |= SF_FORMAT_PCM_32;
    } else if(strcmp(encoding, "pcm_s8")==0) {
        p->sfinfo.format |= SF_FORMAT_PCM_S8;
    } else if(strcmp(encoding, "pcm_u8")==0) {
        p->sfinfo.format |= SF_FORMAT_PCM_U8;
    } else if(strcmp(encoding, "float")==0) {
        p->sfinfo.format |= SF_FORMAT_FLOAT;
    } else if(strcmp(encoding, "double")==0) {
        p->sfinfo.format |= SF_FORMAT_DOUBLE;
    } else if(strcmp(encoding, "ulaw")==0) {
        p->sfinfo.format |= SF_FORMAT_ULAW;
    } else if(strcmp(encoding, "alaw")==0) {
        p->sfinfo.format |= SF_FORMAT_ALAW;
    } else if(strcmp(encoding, "ima_adpcm")==0) {
        p->sfinfo.format |= SF_FORMAT_IMA_ADPCM;
    } else if(strcmp(encoding, "ms_adpcm")==0) {
        p->sfinfo.format |= SF_FORMAT_MS_ADPCM;
    } else if(strcmp(encoding, "gsm610")==0) {
        p->sfinfo.format |= SF_FORMAT_GSM610;
    } else if(strcmp(encoding, "vox_adpcm")==0) {
        p->sfinfo.format |= SF_FORMAT_VOX_ADPCM;
    } else if(strcmp(encoding, "g721_32")==0) {
        p->sfinfo.format |= SF_FORMAT_G721_32;
    } else if(strcmp(encoding, "g723_24")==0) {
        p->sfinfo.format |= SF_FORMAT_G723_24;
    } else if(strcmp(encoding, "g723_40")==0) {
        p->sfinfo.format |= SF_FORMAT_G723_40;
    } else if(strcmp(encoding, "dwvw_12")==0) {
        p->sfinfo.format |= SF_FORMAT_DWVW_12;
    } else if(strcmp(encoding, "dwvw_16")==0) {
        p->sfinfo.format |= SF_FORMAT_DWVW_16;
    } else if(strcmp(encoding, "dwvw_24")==0) {
        p->sfinfo.format |= SF_FORMAT_DWVW_24;
    } else if(strcmp(encoding, "dwvw_n")==0) {
        p->sfinfo.format |= SF_FORMAT_DWVW_N;
    } else if(strcmp(encoding, "dpcm_8")==0) {
        p->sfinfo.format |= SF_FORMAT_DPCM_8;
    } else if(strcmp(encoding, "dpcm_16")==0) {
        p->sfinfo.format |= SF_FORMAT_DPCM_16;
    } else if(strcmp(encoding, "vorbis")==0) {
        p->sfinfo.format |= SF_FORMAT_VORBIS;
    } else {
       Tcl_Free((char *)p);

       Tcl_AppendResult(interp, "encoding unknown option", (char*)0);
       return TCL_ERROR;
    }
  }

  zFile = Tcl_TranslateFileName(interp, zFile, &translatedFilename);
  p->sndfile = sf_open(zFile, p->mode, & (p->sfinfo));
  Tcl_DStringFree(&translatedFilename);

  if(p->sndfile == NULL) {
      Tcl_Free((char *)p);  //open fail, so we need free our memory
      p = NULL;

      return TCL_ERROR;
  }

  p->short_block = NULL;
  p->int_block = NULL;
  p->float_block = NULL;
  p->double_block = NULL;

  switch (p->sfinfo.format & SF_FORMAT_TYPEMASK) {
      case SF_FORMAT_WAV:
          fileformat = "wav";
          break;
      case SF_FORMAT_AIFF:
          fileformat = "aiff";
          break;
      case SF_FORMAT_AU:
          fileformat = "au";
          break;
      case SF_FORMAT_RAW:
          fileformat = "raw";
          break;
      case SF_FORMAT_PAF:
          fileformat = "paf";
          break;
      case SF_FORMAT_SVX:
          fileformat = "svx";
          break;
      case SF_FORMAT_NIST:
          fileformat = "nist";
          break;
      case SF_FORMAT_VOC:
          fileformat = "voc";
          break;
      case SF_FORMAT_IRCAM:
          fileformat = "ircam";
          break;
      case SF_FORMAT_W64:
          fileformat = "w64";
          break;
      case SF_FORMAT_MAT4:
          fileformat = "mat4";
          break;
      case SF_FORMAT_MAT5:
          fileformat = "mat5";
          break;
      case SF_FORMAT_PVF:
          fileformat = "pvf";
          break;
      case SF_FORMAT_XI:
          fileformat = "xi";
          break;
      case SF_FORMAT_HTK:
          fileformat = "htk";
          break;
      case SF_FORMAT_SDS:
          fileformat = "sds";
          break;
      case SF_FORMAT_AVR:
          fileformat = "avr";
          break;
      case SF_FORMAT_WAVEX:
          fileformat = "wavex";
          break;
      case SF_FORMAT_SD2:
          fileformat = "sd2";
          break;
      case SF_FORMAT_FLAC:
          fileformat = "flac";
          break;
      case SF_FORMAT_CAF:
          fileformat = "caf";
          break;
      case SF_FORMAT_WVE:
          fileformat = "wve";
          break;
      case SF_FORMAT_OGG:
          fileformat = "ogg";
          break;
      case SF_FORMAT_MPC2K:
          fileformat = "mpc2k";
          break;
      case SF_FORMAT_RF64:
          fileformat = "rf64";
          break;
      default:
          fileformat = "unknown";
          break;
  }

  switch (p->sfinfo.format & SF_FORMAT_SUBMASK) {
      case SF_FORMAT_PCM_16:
          encoding = "pcm_16";
          break;
      case SF_FORMAT_PCM_24:
          encoding = "pcm_24";
          break;
      case SF_FORMAT_PCM_32:
          encoding = "pcm_32";
          break;
      case SF_FORMAT_PCM_S8:
          encoding = "pcm_s8";
          break;
      case SF_FORMAT_PCM_U8:
          encoding = "pcm_u8";
          break;
      case SF_FORMAT_FLOAT:
          encoding = "float";
          break;
      case SF_FORMAT_DOUBLE:
          encoding = "double";
          break;
      case SF_FORMAT_ULAW:
          encoding = "ulaw";
          break;
      case SF_FORMAT_ALAW:
          encoding = "alaw";
          break;
      case SF_FORMAT_IMA_ADPCM:
          encoding = "ima_adpcm";
          break;
      case SF_FORMAT_MS_ADPCM:
          encoding = "ms_adpcm";
          break;
      case SF_FORMAT_GSM610:
          encoding = "gsm610";
          break;
      case SF_FORMAT_VOX_ADPCM:
          encoding = "vox_adpcm";
          break;
      case SF_FORMAT_G721_32:
          encoding = "g721_32";
          break;
      case SF_FORMAT_G723_24:
          encoding = "g723_24";
          break;
      case SF_FORMAT_G723_40:
          encoding = "g723_40";
          break;
      case SF_FORMAT_DWVW_12:
          encoding = "dwvw_12";
          break;
      case SF_FORMAT_DWVW_16:
          encoding = "dwvw_16";
          break;
      case SF_FORMAT_DWVW_24:
          encoding = "dwvw_24";
          break;
      case SF_FORMAT_DWVW_N:
          encoding = "dwvw_n";
          break;
      case SF_FORMAT_DPCM_8:
          encoding = "dpcm_8";
          break;
      case SF_FORMAT_DPCM_16:
          encoding = "dpcm_16";
          break;
      case SF_FORMAT_VORBIS:
          encoding = "vorbis";
          break;
      default:
          encoding = "unknown";
          break;
  }

  zArg = Tcl_GetStringFromObj(objv[1], 0);
  Tcl_CreateObjCommand(interp, zArg, SndObjCmd, (char*)p, (Tcl_CmdDeleteProc *)NULL);

  /*
   * sfinfo.frames is used to be called samples, not sure is OK for WRITE.
   */
  pResultStr = Tcl_NewListObj(0, NULL);
  Tcl_ListObjAppendElement(interp, pResultStr, Tcl_NewStringObj("frames", -1));
  Tcl_ListObjAppendElement(interp, pResultStr, Tcl_NewIntObj(p->sfinfo.frames));
  Tcl_ListObjAppendElement(interp, pResultStr, Tcl_NewStringObj("fileformat", -1));
  Tcl_ListObjAppendElement(interp, pResultStr, Tcl_NewStringObj(fileformat, -1));
  Tcl_ListObjAppendElement(interp, pResultStr, Tcl_NewStringObj("encoding", -1));
  Tcl_ListObjAppendElement(interp, pResultStr, Tcl_NewStringObj(encoding, -1));
  Tcl_ListObjAppendElement(interp, pResultStr, Tcl_NewStringObj("samplerate", -1));
  Tcl_ListObjAppendElement(interp, pResultStr, Tcl_NewIntObj(p->sfinfo.samplerate));
  Tcl_ListObjAppendElement(interp, pResultStr, Tcl_NewStringObj("channels", -1));
  Tcl_ListObjAppendElement(interp, pResultStr, Tcl_NewIntObj(p->sfinfo.channels));

  Tcl_SetObjResult(interp, pResultStr);  
  return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * Sndfile_Init --
 *
 *	Initialize the new package.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	The Sndfile package is created.
 *
 *----------------------------------------------------------------------
 */

int
Sndfile_Init(Tcl_Interp *interp)
{
    if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL) {
	return TCL_ERROR;
    }
    if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK) {
	return TCL_ERROR;
    }

    Tcl_CreateObjCommand(interp, "sndfile", (Tcl_ObjCmdProc *) SndMain,
        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

    return TCL_OK;
}
