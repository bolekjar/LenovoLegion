// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include <string>
#include <vector>


namespace LenovoLegionDaemon {

/*------------------------------------------------------------------*\
| RGB Color Type and Conversion Macros                               |
\*------------------------------------------------------------------*/
typedef unsigned int RGBColor;


#define RGBGetRValue(rgb)   (rgb & 0x000000FF)
#define RGBGetGValue(rgb)   ((rgb >> 8) & 0x000000FF)
#define RGBGetBValue(rgb)   ((rgb >> 16) & 0x000000FF)

#define ToRGBColor(r, g, b) ((LenovoLegionDaemon::RGBColor)((b << 16) | (g << 8) | (r)))


/*------------------------------------------------------------------*\
| Mode Flags                                                         |
\*------------------------------------------------------------------*/
enum
{
    MODE_FLAG_HAS_SPEED                 = (1 << 0), /* Mode has speed parameter         */
    MODE_FLAG_HAS_DIRECTION_LR          = (1 << 1), /* Mode has left/right parameter    */
    MODE_FLAG_HAS_DIRECTION_UD          = (1 << 2), /* Mode has up/down parameter       */
    MODE_FLAG_HAS_DIRECTION_HV          = (1 << 3), /* Mode has horiz/vert parameter    */
    MODE_FLAG_HAS_BRIGHTNESS            = (1 << 4), /* Mode has brightness parameter    */
    MODE_FLAG_HAS_PER_LED_COLOR         = (1 << 5), /* Mode has per-LED colors          */
    MODE_FLAG_HAS_MODE_SPECIFIC_COLOR   = (1 << 6), /* Mode has mode specific colors    */
    MODE_FLAG_HAS_RANDOM_COLOR          = (1 << 7), /* Mode has random color option     */
    MODE_FLAG_AUTOMATIC_SAVE            = (1 << 8), /* Mode automatically saves         */
    MODE_FLAGS_DIRECT                   = (1 << 9), /* Mode is direct mode              */
};

/*------------------------------------------------------------------*\
| Mode Directions                                                    |
\*------------------------------------------------------------------*/
enum
{
    MODE_DIRECTION_LEFT         = 0,        /* Mode direction left              */
    MODE_DIRECTION_RIGHT        = 1,        /* Mode direction right             */
    MODE_DIRECTION_UP           = 2,        /* Mode direction up                */
    MODE_DIRECTION_DOWN         = 3,        /* Mode direction down              */
    MODE_DIRECTION_HORIZONTAL   = 4,        /* Mode direction horizontal        */
    MODE_DIRECTION_VERTICAL     = 5,        /* Mode direction vertical          */
};

/*------------------------------------------------------------------*\
| Mode Color Types                                                   |
\*------------------------------------------------------------------*/
enum
{
    MODE_COLORS_NONE            = 0,        /* Mode has no colors               */
    MODE_COLORS_PER_LED         = 1,        /* Mode has per LED colors selected */
    MODE_COLORS_MODE_SPECIFIC   = 2,        /* Mode specific colors selected    */
    MODE_COLORS_RANDOM          = 3,        /* Mode has random colors selected  */
};

/*------------------------------------------------------------------*\
| Mode Class                                                         |
\*------------------------------------------------------------------*/
class mode
{
public:
    /*--------------------------------------------------------------*\
    | Mode Information                                               |
    \*--------------------------------------------------------------*/
    std::string         name;   /* Mode name                        */
    int                 value;  /* Device-specific mode value       */
    unsigned int        flags;  /* Mode flags bitfield              */
    unsigned int        speed_min;  /* speed minimum value          */
    unsigned int        speed_max;  /* speed maximum value          */
    unsigned int        brightness_min; /*brightness min value      */
    unsigned int        brightness_max; /*brightness max value      */
    unsigned int        colors_min; /* minimum number of mode colors*/
    unsigned int        colors_max; /* maximum numver of mode colors*/

    /*--------------------------------------------------------------*\
    | Mode Settings                                                  |
    \*--------------------------------------------------------------*/
    unsigned int        speed;  /* Mode speed parameter value       */
    unsigned int        brightness; /* Mode brightness value        */
    unsigned int        direction;  /* Mode direction value         */
    unsigned int        color_mode; /* Mode color selection         */
    std::vector<RGBColor>
        colors; /* mode-specific colors             */

    /*--------------------------------------------------------------*\
    | Mode Constructor / Destructor                                  |
    \*--------------------------------------------------------------*/
    mode()
    {
        name           = "";
        value          = 0;
        flags          = 0;
        speed_min      = 0;
        speed_max      = 0;
        brightness_min = 0;
        brightness_max = 0;
        colors_min     = 0;
        colors_max     = 0;
        speed          = 0;
        brightness     = 0;
        direction      = 0;
        color_mode     = 0;
    }

    ~mode()
    {
        colors.clear();
    }
};

/*------------------------------------------------------------------*\
| LED Struct                                                         |
\*------------------------------------------------------------------*/
typedef struct
{
    std::string         name;   /* LED name                     */
    unsigned int        value;  /* Device-specific LED value    */
} led;

/*------------------------------------------------------------------*\
| Zone Flags                                                         |
\*------------------------------------------------------------------*/
enum
{
    ZONE_FLAG_RESIZE_EFFECTS_ONLY       = (1 << 0), /* Zone is resizable, but only for  */
    /* effects - treat as single LED    */
};

/*------------------------------------------------------------------*\
| Zone Types                                                         |
\*------------------------------------------------------------------*/
typedef int zone_type;

enum
{
    ZONE_TYPE_SINGLE,
    ZONE_TYPE_LINEAR,
    ZONE_TYPE_MATRIX
};

/*------------------------------------------------------------------*\
| Matrix Map Struct                                                  |
\*------------------------------------------------------------------*/
typedef struct
{
    unsigned int            height;
    unsigned int            width;
    unsigned int *          map;
} matrix_map_type;

/*------------------------------------------------------------------*\
| Segment Struct                                                     |
\*------------------------------------------------------------------*/
typedef struct
{
    std::string             name;           /* Segment name             */
    zone_type               type;           /* Segment type             */
    unsigned int            start_idx;      /* Start index within zone  */
    unsigned int            leds_count;     /* Number of LEDs in segment*/
} segment;

/*------------------------------------------------------------------*\
| Zone Class                                                         |
\*------------------------------------------------------------------*/
class zone
{
public:
    std::string             name;           /* Zone name                */
    zone_type               type;           /* Zone type                */
    led *                   leds;           /* List of LEDs in zone     */
    RGBColor *              colors;         /* Colors of LEDs in zone   */
    unsigned int            start_idx;      /* Start index of led/color */
    unsigned int            leds_count;     /* Number of LEDs in zone   */
    unsigned int            leds_min;       /* Minimum number of LEDs   */
    unsigned int            leds_max;       /* Maximum number of LEDs   */
    matrix_map_type*        matrix_map;     /* Matrix map pointer       */
    std::vector<segment>    segments;       /* Segments in zone         */
    unsigned int            flags;          /* Zone flags bitfield      */

    /*--------------------------------------------------------------*\
    | Zone Constructor / Destructor                                  |
    \*--------------------------------------------------------------*/
    zone()
    {
        name        = "";
        type        = 0;
        leds        = NULL;
        colors      = NULL;
        start_idx   = 0;
        leds_count  = 0;
        leds_min    = 0;
        leds_max    = 0;
        flags       = 0;
    }

    zone(zone &&other)
    {
        name        = other.name;
        type        = other.type;
        leds        = other.leds;
        colors      = other.colors;
        start_idx   = other.start_idx;
        leds_count  = other.leds_count;
        leds_min    = other.leds_min;
        leds_max    = other.leds_max;
        matrix_map  = other.matrix_map;
        segments    = std::move(other.segments);
        flags       = other.flags;

        other.leds       = NULL;
        other.colors     = NULL;
        other.matrix_map = NULL;
    }

    ~zone()
    {}

    zone(const zone&)  = delete;
    zone& operator=(const zone&)  = delete;
    zone& operator=(const zone&&) = delete;

};

/*------------------------------------------------------------------*\
| Device Types                                                       |
|   The enum order should be maintained as is for the API however    |
|   DEVICE_TYPE_UNKNOWN needs to remain last. Any new device types   |
|   need to be inserted at the end of the list but before unknown.   |
\*------------------------------------------------------------------*/
typedef int device_type;

enum
{
    DEVICE_TYPE_MOTHERBOARD,
    DEVICE_TYPE_DRAM,
    DEVICE_TYPE_GPU,
    DEVICE_TYPE_COOLER,
    DEVICE_TYPE_LEDSTRIP,
    DEVICE_TYPE_KEYBOARD,
    DEVICE_TYPE_MOUSE,
    DEVICE_TYPE_MOUSEMAT,
    DEVICE_TYPE_HEADSET,
    DEVICE_TYPE_HEADSET_STAND,
    DEVICE_TYPE_GAMEPAD,
    DEVICE_TYPE_LIGHT,
    DEVICE_TYPE_SPEAKER,
    DEVICE_TYPE_VIRTUAL,
    DEVICE_TYPE_STORAGE,
    DEVICE_TYPE_CASE,
    DEVICE_TYPE_MICROPHONE,
    DEVICE_TYPE_ACCESSORY,
    DEVICE_TYPE_KEYPAD,
    DEVICE_TYPE_UNKNOWN,
};

/*------------------------------------------------------------------*\
| Controller Flags                                                   |
\*------------------------------------------------------------------*/
enum
{
    CONTROLLER_FLAG_LOCAL               = (1 << 0), /* Device is local to this instance */
    CONTROLLER_FLAG_REMOTE              = (1 << 1), /* Device is on a remote instance   */
    CONTROLLER_FLAG_VIRTUAL             = (1 << 2), /* Device is a virtual device       */

    CONTROLLER_FLAG_RESET_BEFORE_UPDATE = (1 << 8), /* Device resets update flag before */
    /* calling update function          */
};

/*------------------------------------------------------------------*\
| RGBController Callback Types                                       |
\*------------------------------------------------------------------*/
typedef void (*RGBControllerCallback)(void *);

std::string device_type_to_str(device_type type);

class RGBControllerInterface
{
public:
    virtual ~RGBControllerInterface() {}

    /*
     * Getters
     */
    virtual unsigned int                  GetLEDsInZone(unsigned int zone)    const                             = 0;
    virtual std::string                   GetModeName(unsigned int mode)      const                             = 0;
    virtual std::string                   GetZoneName(unsigned int zone)      const                             = 0;
    virtual std::string                   GetLEDName(unsigned int led)        const                             = 0;
    virtual const std::vector<zone>&      GetZones()                          const                             = 0;
    virtual const std::vector<RGBColor>&  GetColors()          const                                            = 0;
    virtual device_type                   GetDeviceType()      const                                            = 0;
    virtual unsigned int                  GetProfiles()        const                                            = 0;
    virtual size_t                        GetActiveProfile()   const                                            = 0;

    /*
     * Getters / Setters
     */
    virtual RGBColor                      GetLED(unsigned int led)          const                               = 0;
    virtual void                          SetLED(unsigned int led, RGBColor color)                              = 0;

    virtual const std::vector<led>&       GetLEDs() const                                                       = 0;
    virtual void                          SetLEDs(const std::vector<led>& new_leds)                             = 0;

    virtual int                           GetMode()      const                                                  = 0;
    virtual void                          SetMode(int mode)                                                     = 0;

    virtual const std::vector<mode>&      GetModes()           const                                            = 0;
    virtual void                          SetModes(const std::vector<mode>& new_modes)                          = 0;

    /*
     * Setters
     */
    virtual void                          SetAllLEDs(RGBColor color)                                            = 0;
    virtual void                          SetProfile(size_t profileIdx)                                         = 0;

    virtual void                          ApplyPendingChanges() {};
};

}
