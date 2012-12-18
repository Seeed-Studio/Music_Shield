/** \file ui.h User Interface common definitions - functions are still in player.c */

#ifndef UI_H
#define UI_H

/** User Interface Modes */
typedef enum {
  UI_TITLE,        /**< Track, title and playing time  */
  UI_SPEC,         /**< Spectrum Analyzer              */
  UI_VOLUME,       /**< Volume setting                 */
  UI_BASS,         /**< Bass enhancer setting          */
  UI_TREBLE,       /**< Treble enhancer setting        */
  UI_CUE,          /**< ask to FF/REW?                 */
  UI_RECLEVEL,     /**< Recording Level setting        */
  UI_INFO,         /**< Decoding info                  */		 
  UI_STOP,         /**< Stop and return to browser?    */
  UI_END_OF_MODES  /**< Nothing, jump go to first mode */
} uimodetype;
extern  uimodetype uiMode;

/** Playing states. \todo safe rewind */
typedef enum {
  PS_NORMAL = 0,    /**< Continue playing, do nothing special      */
  PS_END_OF_SONG,   /**< Request termination of current song       */
  PS_NEXT_SONG,     /**< Request next song                         */
  PS_PREVIOUS_SONG, /**< Request previous song                     */
  PS_CUE,           /**< Request to start fast forward             */
  PS_CUE_WAIT1003,  /**< WMA: wait for permission to break stream  */
  PS_CUE_ACTION,    /**< Just do it. */
  PS_REWIND,        /**< Request to start rewind (not implemented) */
  PS_REW_WAIT1003,  /**< WMA: wait for permission to break stream  */
  PS_FALLBACK_1,    /**< Start fallback to normal after ff/rew     */
  PS_FALLBACK_N,    /**< fallbackCount sectors to PS_NORMAL        */
  PS_RECORDING      /**< Recording mode */

} playingstatetype;
extern  playingstatetype playingState;

#endif

