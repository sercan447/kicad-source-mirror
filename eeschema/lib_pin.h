/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015 Jean-Pierre Charras, jaen-pierre.charras at wanadoo.fr
 * Copyright (C) 2015 Wayne Stambaugh <stambaughw@verizon.net>
 * Copyright (C) 2004-2015 KiCad Developers, see change_log.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

/**
 * @file class_libentry.h
 * @brief Class LIB_PIN definition.
 */
#ifndef CLASS_PIN_H
#define CLASS_PIN_H

#include <lib_draw_item.h>


#define TARGET_PIN_RADIUS   12  // Circle diameter drawn at the active end of pins

/**
 * The component library pin object electrical types used in ERC tests.
 */
enum ElectricPinType {
    PIN_INPUT,
    PIN_OUTPUT,
    PIN_BIDI,
    PIN_TRISTATE,
    PIN_PASSIVE,
    PIN_UNSPECIFIED,
    PIN_POWER_IN,
    PIN_POWER_OUT,
    PIN_OPENCOLLECTOR,
    PIN_OPENEMITTER,
    PIN_NC,             /* No connect */
    PIN_NMAX            /* End of List (no used as pin type) */
};


/* Pin visibility flag bit. */
#define PIN_INVISIBLE 1    /* Set makes pin invisible */


/**
 * The component library pin object drawing shapes.
 */
enum DrawPinShape {
    NONE         = 0,
    INVERT       = 1,
    CLOCK        = 2,
    LOWLEVEL_IN  = 4,
    LOWLEVEL_OUT = 8,
    CLOCK_FALL   = 0x10, /* this is common form for inverted clock in Eastern Block */
    NONLOGIC     = 0x20
};


/**
 *  The component library pin object orientations.
 */
enum DrawPinOrient {
    PIN_RIGHT = 'R',
    PIN_LEFT  = 'L',
    PIN_UP    = 'U',
    PIN_DOWN  = 'D'
};

enum LibPinDrawFlags {
    PIN_DRAW_TEXTS = 1,
    PIN_DRAW_DANGLING = 2,
};


class LIB_PIN : public LIB_ITEM
{
    wxPoint  m_position;     ///< Position of the pin.
    int      m_length;       ///< Length of the pin.
    int      m_orientation;  ///< Pin orientation (Up, Down, Left, Right)
    int      m_shape;        ///< Bitwise ORed of pin shapes (see enum DrawPinShape)
    int      m_width;        ///< Line width of the pin.
    int      m_type;         ///< Electrical type of the pin.  See enum ElectricPinType.
    int      m_attributes;   ///< Set bit 0 to indicate pin is invisible.
    wxString m_name;
    long     m_number;       ///< Pin number defined as 4 ASCII characters like "12", "anod",
                             ///< "G6", or "12".  It is stored as "12\0\0" and does not
                             ///< depend on endian type.
    int      m_numTextSize;
    int      m_nameTextSize; ///< Pin num and Pin name sizes

    /**
     * Draw a pin, with or without the pin texts
     *
     * @param aPanel DrawPanel to use (can be null) mainly used for clipping purposes.
     * @param aDC Device Context (can be null)
     * @param aOffset Offset to draw
     * @param aColor -1 to use the normal body item color, or use this color if >= 0
     * @param aDrawMode GR_OR, GR_XOR, ...
     * @param aData = used here as uintptr_t containing bitwise OR'd flags:
     *      PIN_DRAW_TEXTS,     -- false to draw only pin shape, useful for fast mode
     *      PIN_DRAW_DANGLING,
     * @param aTransform Transform Matrix (rotation, mirror ..)
     */
    void drawGraphic( EDA_DRAW_PANEL* aPanel, wxDC* aDC, const wxPoint& aOffset,
                      EDA_COLOR_T aColor, GR_DRAWMODE aDrawMode, void* aData,
                      const TRANSFORM& aTransform );

public:
    LIB_PIN( LIB_PART*      aParent );

    // Do not create a copy constructor.  The one generated by the compiler is adequate.

    ~LIB_PIN() { }

    wxString GetClass() const
    {
        return wxT( "LIB_PIN" );
    }

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const;   // virtual override
#endif

    bool Save( OUTPUTFORMATTER& aFormatter );

    bool Load( LINE_READER& aLineReader, wxString& aErrorMsg );

    bool HitTest( const wxPoint& aPosition ) const;

    bool HitTest( const wxPoint &aPosRef, int aThreshold, const TRANSFORM& aTransform ) const;

    void GetMsgPanelInfo( std::vector< MSG_PANEL_ITEM >& aList );

    bool Matches( wxFindReplaceData& aSearchData, void* aAuxData, wxPoint* aFindLocation );

    const EDA_RECT GetBoundingBox() const;    // Virtual

    /**
     * Function PinEndPoint
     *
     * @return The pin end position for a component in the normal orientation.
     */
    wxPoint PinEndPoint() const;

    /**
     * Function PinDrawOrient
     * returns the pin real orientation (PIN_UP, PIN_DOWN, PIN_RIGHT, PIN_LEFT),
     * according to its orientation and the matrix transform (rot, mirror) \a aTransform
     *
     * @param aTransform Transform matrix
     */
    int PinDrawOrient( const TRANSFORM& aTransform ) const;

    /**
     * Fill a string buffer with pin number.
     *
     * Pin numbers are coded as a long or 4 ASCII characters.  Used to print
     * or draw the pin number.
     *
     * @param aStringBuffer - the wxString to store the pin num as an unicode string
     */
    void PinStringNum( wxString& aStringBuffer ) const;

    long GetNumber() const { return m_number; }

    wxString GetNumberString() const { return PinStringNum( m_number ); }

    /**
     * Function PinStringNum (static function)
     * Pin num is coded as a long or 4 ascii chars
     * @param aPinNum = a long containing a pin num
     * @return aStringBuffer = the wxString to store the pin num as an
     *                         unicode string
     */
    static wxString PinStringNum( long aPinNum );

    /**
     * Function SetPinNumFromString
     * fill the pin number buffer with \a aBuffer.
     */
    void SetPinNumFromString( wxString& aBuffer );

    wxString GetName() const { return m_name; }

    /**
     * Set the pin name.
     *
     * This will also all of the pin names marked by EnableEditMode().
     *
     * @param aName New pin name.
     */
    void SetName( const wxString& aName );

    /**
     * Set the \a aSize of the pin name text.
     *
     * This will also update the text size of the name of the pins marked
     * by EnableEditMode().
     *
     * @param aSize The text size of the pin name in schematic units ( mils ).
     */
    void SetNameTextSize( int aSize );

    int GetNameTextSize() const { return m_nameTextSize; }

    /**
     * Set the pin number.
     *
     * Others pin numbers marked by EnableEditMode() are not modified
     * because each pin has its own number
     * @param aNumber New pin number.
     */
    void SetNumber( const wxString& aNumber );

    /**
     * Set the size of the pin number text.
     *
     * This will also update the text size of the number of the pins marked
     * by EnableEditMode().
     *
     * @param aSize The text size of the pin number in schematic units ( mils ).
     */
    void SetNumberTextSize( int aSize );

    int GetNumberTextSize() const { return m_numTextSize; }

    int GetOrientation() const { return m_orientation; }

    /**
     * Set orientation on the pin.
     *
     * This will also update the orientation of the pins marked by EnableEditMode().
     *
     * @param aOrientation - The orientation of the pin.
     */
    void SetOrientation( int aOrientation );

    void Rotate();

    int GetShape() const { return m_shape; }

    /**
     * Set the shape of the pin to \a aShape.
     *
     * This will also update the draw style of the pins marked by EnableEditMode().
     *
     * @param aShape - The draw shape of the pin.  See enum DrawPinShape.
     */
    void SetShape( int aShape );

    /**
     * Get the electrical type of the pin.
     *
     * @return The electrical type of the pin (see enun ElectricPinType for values).
     */
    int GetType() const { return m_type; }

    /**
     * return a string giving the electrical type of a pin.
     * Can be used when a known, not translated name is needed (for instance in net lists)
     * @param aType is the electrical type (see enum ElectricPinType )
     * @return The electrical name for a pin type (see enun MsgPinElectricType for names).
     */
    static const wxString GetCanonicalElectricalTypeName( unsigned aType );

    /**
     * return a string giving the electrical type of the pin.
     * Can be used when a known, not translated name is needed (for instance in net lists)
     * @return The canonical electrical name of the pin.
     */
    wxString const GetCanonicalElectricalTypeName() const
    {
        return GetCanonicalElectricalTypeName( m_type );
    }

    /**
     * return a translated string for messages giving the electrical type of a pin.
     * @param aType is the electrical type (see enum ElectricPinType )
     * @return The electrical name of the pin (see enun MsgPinElectricType for names).
     */
    static const wxString GetElectricalTypeName( unsigned aType );

    /**
     * return a translated string for messages giving the electrical type of the pin.
     * @return The electrical name of the pin.
     */
    wxString const GetElectricalTypeName() const
    {
        return GetElectricalTypeName( m_type );
    }

    /**
     * Set the electrical type of the pin.
     *
     * This will also update the electrical type of the pins marked by
     * EnableEditMode().
     *
     * @param aType - The electrical type of the pin(see enun ElectricPinType for values).
     */
    void SetType( int aType );

    /**
     * Set the pin length.
     *
     * This will also update the length of the pins marked by EnableEditMode().
     *
     * @param aLength - The length of the pin in mils.
     */
    void SetLength( int aLength );

    int GetLength() { return m_length; }

    /**
     * Set the pin part number.
     *
     * If the pin is changed from not common to common to all parts, any
     * linked pins will be removed from the parent component.
     *
     * @param aPart - Number of the part the pin belongs to.  Set to zero to
     *                make pin common to all parts in a multi-part component.
     */
    void SetPartNumber( int aPart );

    /** Get the pin part number. */
    int GetPartNumber() const { return m_Unit; }

    /**
     * Set the body style (conversion) of the pin.
     *
     * If the pin is changed from not common to common to all body styles, any
     * linked pins will be removed from the parent component.
     *
     * @param aConversion - Body style of the pin.  Set to zero to make pin
     *                      common to all body styles.
     */
    void SetConversion( int aConversion );

    /**
     * Set or clear the visibility flag for the pin.
     *
     * This will also update the visibility of the pins marked by
     * EnableEditMode().
     *
     * @param aVisible - True to make the pin visible or false to hide the pin.
     */
    void SetVisible( bool aVisible );

    /**
     * Enable or clear pin editing mode.
     *
     * The pin editing mode marks or unmarks all pins common to this
     * pin object for further editing.  If any of the pin modification
     * methods are called after enabling the editing mode, all pins
     * marked for editing will have the same attribute changed.  The
     * only case were this is not true making this pin common to all
     * parts or body styles in the component.  See SetCommonToAllParts()
     * and SetCommonToAllBodyStyles() for more information.
     *
     * @param aEnable True marks all common pins for editing mode.  False
     *                clears the editing mode.
     * @param aEditPinByPin Enables the edit pin by pin mode.
     */
    void EnableEditMode( bool aEnable, bool aEditPinByPin = false );

    /**
     * Return the visibility status of the draw object.
     *
     * @return True if draw object is visible otherwise false.
     */
    bool IsVisible() const { return ( m_attributes & PIN_INVISIBLE ) == 0; }

    /**
     * Return whether this pin forms an implicit power connection: i.e., is hidden
     * and of type POWER_IN.
     */
    bool IsPowerConnection() const { return !IsVisible() && GetType() == PIN_POWER_IN; }

    int GetPenSize() const;

    /**
     * Function DrawPinSymbol
     * Draw the pin symbol without text.
     * If \a aColor != 0, draw with \a aColor, else with the normal pin color.
     */
    void DrawPinSymbol( EDA_DRAW_PANEL* aPanel, wxDC* aDC, const wxPoint& aPosition,
                        int aOrientation, GR_DRAWMODE aDrawMode,
                        EDA_COLOR_T aColor = UNSPECIFIED_COLOR,
                        bool aDrawDangling = true );

    /**
     * Function DrawPinTexts
     * puts the pin number and pin text info, given the pin line coordinates.
     * The line must be vertical or horizontal.
     * If DrawPinName == false the pin name is not printed.
     * If DrawPinNum = false the pin number is not printed.
     * If TextInside then the text is been put inside,otherwise all is drawn outside.
     * Pin Name:    substring between '~' is negated
     * DrawMode = GR_OR, XOR ...
     */
    void DrawPinTexts( EDA_DRAW_PANEL* aPanel, wxDC* aDC, wxPoint& aPosition,
                       int aOrientation, int TextInside, bool DrawPinNum, bool DrawPinName,
                       EDA_COLOR_T aColor, GR_DRAWMODE aDrawMode );

    /**
     * Function PlotPinTexts
     * plots the pin number and pin text info, given the pin line coordinates.
     * Same as DrawPinTexts((), but output is the plotter
     * The line must be vertical or horizontal.
     * If TextInside then the text is been put inside (moving from x1, y1 in
     * the opposite direction to x2,y2), otherwise all is drawn outside.
     */
    void PlotPinTexts( PLOTTER *aPlotter,
                       wxPoint& aPosition,
                       int      aOrientation,
                       int      aTextInside,
                       bool     aDrawPinNum,
                       bool     aDrawPinName,
                       int      aWidth );

    void PlotSymbol( PLOTTER* aPlotter, const wxPoint& aPosition, int aOrientation );

    /**
     * Get a list of pin orientation names.
     *
     * @return List of valid pin orientation names.
     */
    static wxArrayString GetOrientationNames();

    /**
     * Get a list of pin orientation bitmaps for menus and dialogs.
     *
     * @return  List of valid pin orientation bitmaps symbols in .xpm format
     */
    static const BITMAP_DEF* GetOrientationSymbols();

    /**
     * Get the orientation code by index used to set the pin orientation.
     *
     * @param aIndex - The index of the orientation code to look up.
     * @return Orientation code if index is valid.  Returns right
     *         orientation on index error.
     */
    static int GetOrientationCode( int aIndex );

    /**
     * Get the index of the orientation code.
     *
     * @param aCode - The orientation code to look up.
     * @return  The index of the orientation code if found.  Otherwise,
     *          return wxNOT_FOUND.
     */
    static int GetOrientationCodeIndex( int aCode );

    /**
     * Get a list of pin draw style names.
     *
     * @return  List of valid pin draw style names.
     */
    static wxArrayString GetStyleNames();

    /**
     * Get a list of pin styles bitmaps for menus and dialogs.
     *
     * @return  List of valid pin electrical type bitmaps symbols in .xpm format.
     */
    static const BITMAP_DEF* GetStyleSymbols();

    /**
     * Get the pin draw style code by index used to set the pin draw style.
     *
     * @param aIndex - The index of the pin draw style code to look up.
     * @return  Pin draw style code if index is valid.  Returns NONE
     *          style on index error.
     */
    static int GetStyleCode( int aIndex );

    /**
     * Get the index of the pin draw style code.
     *
     * @param aCode - The pin draw style code to look up.
     * @return The index of the pin draw style code if found.  Otherwise,
     *         return wxNOT_FOUND.
     */
    static int GetStyleCodeIndex( int aCode );

    /**
     * Get a list of pin electrical type names.
     *
     * @return  List of valid pin electrical type names.
     */
    static wxArrayString GetElectricalTypeNames();

    /**
     * Get a list of pin electrical bitmaps for menus and dialogs.
     *
     * @return  List of valid pin electrical type bitmaps symbols in .xpm format
     */
    static const BITMAP_DEF* GetElectricalTypeSymbols();

    void SetOffset( const wxPoint& aOffset );

    bool Inside( EDA_RECT& aRect ) const;

    void Move( const wxPoint& aPosition );

    wxPoint GetPosition() const { return m_position; }

    void MirrorHorizontal( const wxPoint& aCenter );

    void MirrorVertical( const wxPoint& aCenter );

    void Rotate( const wxPoint& aCenter, bool aRotateCCW = true );

    void Plot( PLOTTER* aPlotter, const wxPoint& aOffset, bool aFill,
               const TRANSFORM& aTransform );

    int GetWidth() const { return m_width; }

    void SetWidth( int aWidth );

    BITMAP_DEF GetMenuImage() const;

    wxString GetSelectMenuText() const;

    EDA_ITEM* Clone() const;

private:

    /**
     * @copydoc LIB_ITEM::compare()
     *
     * The pin specific sort order is as follows:
     *      - Pin number.
     *      - Pin name, case insensitive compare.
     *      - Pin horizontal (X) position.
     *      - Pin vertical (Y) position.
     */
    int compare( const LIB_ITEM& aOther ) const;
};


#endif  //  CLASS_PIN_H
