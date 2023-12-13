#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/notebook.h>
#include <wx/listctrl.h>
#include <wx/filedlg.h>
#include <wx/image.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/combobox.h>
#include <wx/rearrangectrl.h>
#include <vector>

#include "engine.hpp"
#include "sprite.hpp"
#include "physics.hpp"
#define ScreenWidth 600
#define ScreenHeight 600
#define ScrollSpeed 1
#define ScrollSpeed2 2

typedef enum
{
    NEW,
    OPEN,
    SAVE,
    PLAY,
} EditorMenuItems;

class EditorApp : public wxApp
{
public:
    bool OnInit() override;
};

class LayerPropertiesDlg : public wxFrame
{
public:
    int list_item_modified;
    int num_list_items;

    LayerPropertiesDlg(wxWindow *parent, wxString layer_path, wxString layer_scroll, int item_num, int max_item);
    wxString GetNewPath();
    wxString GetNewScroll();
    int GetNewOrder();

private:
    wxGridSizer *layout;
    wxString filepath;
    wxSpinCtrlDouble *speed_ctrl;
    wxSpinCtrl *order_ctrl;

    wxStaticText *file_path_screentext;

    void OnChangeFile(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
};

class TilePropertiesDlg : public wxFrame
{
public:
    int idx;

    TilePropertiesDlg(wxWindow *parent, int idx, wxString tile_path, wxString Foreground, wxString Collides);
    wxString GetNewPath();
    wxString GetNewForegroundStatus();
    wxString GetNewCollisionStatus();

private:
    wxGridSizer *layout;

    wxString filepath;
    wxStaticText *file_path_screentext;

    wxComboBox *collision_box;
    wxComboBox *foreground_box;

    void OnChangeFile(wxCommandEvent &event);
};

class SpritePropertiesDlg : public wxFrame
{
public:
    int list_index;

    SpritePropertiesDlg(wxWindow *parent, int idx, wxString file, wxString height, wxString width, wxString user_control, wxString spritex, wxString spritey, wxString jump, wxString speed, wxString projbool);
    wxString GetNewPath();
    wxString GetNewHeight();
    wxString GetNewWidth();
    wxString GetNewUserControlStatus();
    wxString GetSpriteX();
    wxString GetSpriteY();
    wxString GetSpriteJump();
    wxString GetSpriteSpeed();
    wxString GetProjectile();

private:
    wxGridSizer *layout;

    wxString filepath;
    wxStaticText *file_path_screentext;

    wxSpinCtrl *width;
    wxSpinCtrl *height;
    wxComboBox *user_ctrl;
    wxSpinCtrl *x;
    wxSpinCtrl *y;
    wxSpinCtrl *jumph;
    wxSpinCtrl *speed;
    wxComboBox *proj;

    void
    OnChangeFile(wxCommandEvent &event);
};

class EditorFrame : public wxFrame
{
public:
    EditorFrame();
    bool should_continue;

private:
    wxString game_filename;

    wxBoxSizer *layout;

    wxPanel *layers_panel;
    wxBoxSizer *layers_layout;
    wxListCtrl *layer_art_list;
    void InitLayersPanel();

    wxPanel *tiles_panel;
    wxBoxSizer *tiles_layout;
    wxListCtrl *tile_arts;
    void InitTilesPanel();

    wxPanel *sprites_panel;
    wxBoxSizer *sprites_layout;
    wxListCtrl *sprites_list;
    void InitSpritesPanel();

    wxNotebook *tools;

    wxMenu *help_menu;
    wxMenu *file_menu;
    wxMenuBar *menubar;

    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);

    void OnNew(wxCommandEvent &event);
    void OnSave(wxCommandEvent &event);
    void OnOpen(wxCommandEvent &event);
    void OnPlay(wxCommandEvent &event);

    TilePropertiesDlg *tp;
    void OnAddTileArt(wxCommandEvent &event);
    void OnRemoveTileArt(wxCommandEvent &event);
    void OnEditTileProps(wxCommandEvent &event);
    void OnTilePropsClose(wxCloseEvent &event);
    void OnPlaceTiles(wxCommandEvent &event);

    LayerPropertiesDlg *lp;
    void OnAddLayerArt(wxCommandEvent &event);
    void OnRemoveLayer(wxCommandEvent &event);
    void OnLayerProps(wxCommandEvent &event);
    void OnLayerPropsClose(wxCloseEvent &event);
    void OnPreviewLayers(wxCommandEvent &event);
    std::vector<std::string> GetLayers();

    SpritePropertiesDlg *sp;
    void OnAddSprite(wxCommandEvent &event);
    void OnRemoveSprite(wxCommandEvent &event);
    void OnSpriteProps(wxCommandEvent &event);
    void OnSpritePropsClose(wxCloseEvent &event);
    void OnPlaceSprites(wxCommandEvent &event);
    int_fast8_t getSprite();
};

wxIMPLEMENT_APP(EditorApp);

#endif