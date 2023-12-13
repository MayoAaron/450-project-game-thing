#include "editor.hpp"
#include <iostream>
#include <cstring>

bool EditorApp::OnInit()
{
    EditorFrame *frame = new EditorFrame();
    frame->Show();

    return true;
}

EditorFrame::EditorFrame() : wxFrame(NULL, wxID_ANY, "Otter Game Engine")
{
    this->SetMinSize(wxSize(800, 600));

    file_menu = new wxMenu;
    file_menu->Append(NEW, "&New");
    file_menu->Append(OPEN, "&Open");
    file_menu->Append(SAVE, "&Save");
    file_menu->AppendSeparator();
    file_menu->Append(PLAY, "&Play");
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT);

    help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT);

    menubar = new wxMenuBar;
    menubar->Append(file_menu, "&File");
    menubar->Append(help_menu, "&Help");
    SetMenuBar(menubar);

    CreateStatusBar();
    SetStatusText("...");

    Bind(wxEVT_MENU, &EditorFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &EditorFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &EditorFrame::OnPlay, this, PLAY);

    layout = new wxBoxSizer(wxHORIZONTAL);
    tools = new wxNotebook(this, wxID_ANY);
    layout->Add(tools);

    InitLayersPanel();
    tools->AddPage(layers_panel, wxT("Background"));

    InitTilesPanel();
    tools->AddPage(tiles_panel, wxT("Tiles"));

    InitSpritesPanel();
    tools->AddPage(sprites_panel, wxT("Sprites"));

    this->game_filename = "";
}

void EditorFrame::InitLayersPanel()
{
    layers_panel = new wxPanel(tools, wxID_ANY);
    layers_layout = new wxBoxSizer(wxVERTICAL);

    layers_panel->SetSizer(layers_layout);

    wxButton *layer_art_bttn = new wxButton(layers_panel, wxID_ANY, "Add Layer");
    layer_art_bttn->Bind(wxEVT_BUTTON, &EditorFrame::OnAddLayerArt, this);

    wxButton *remove_layer_bttn = new wxButton(layers_panel, wxID_ANY, "Remove Selected Layer");
    remove_layer_bttn->Bind(wxEVT_BUTTON, &EditorFrame::OnRemoveLayer, this);

    wxButton *edit_layer_props = new wxButton(layers_panel, wxID_ANY, "Selected Layer Properties");
    edit_layer_props->Bind(wxEVT_BUTTON, &EditorFrame::OnLayerProps, this);

    layer_art_list = new wxListCtrl(layers_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES | wxLC_REPORT);
    layer_art_list->InsertColumn(0, "Filename");
    layer_art_list->InsertColumn(1, "Scroll Speed");
    layer_art_list->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
    layer_art_list->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);

    layers_layout->Add(layer_art_bttn, 0, wxEXPAND | wxALL, 10);
    layers_layout->Add(remove_layer_bttn, 0, wxEXPAND | wxALL, 10);
    layers_layout->Add(edit_layer_props, 0, wxEXPAND | wxALL, 10);
    layers_layout->Add(layer_art_list, 0, wxEXPAND | wxALL, 10);
}

/* TODO: Engine Team
 * Open window & display layers, allow for scrolling with WASD
 */

void EditorFrame::OnPreviewLayers(wxCommandEvent &wx_event)
{
    Disable(); // Disable Editor while displaying testbackground

    std::vector<std::string> layer_filenames;
    std::vector<double> layer_scroll_speeds;

    // Extract layer information from editor
    long item = -1;
    while (true)
    {
        item = layer_art_list->GetNextItem(item);

        if (item == -1)
            break;

        std::string filename = layer_art_list->GetItemText(item).ToStdString();
        layer_filenames.push_back(filename);

        double scroll = std::stod(layer_art_list->GetItemText(item, 1).ToStdString());
        layer_scroll_speeds.push_back(scroll);
    }

    // Looping over layer information
    for (int i = 0; i < layer_filenames.size(); i++)
    {
        std::string filename = layer_filenames.at(i);
        double scroll = layer_scroll_speeds.at(i);

        std::cout << filename << ": " << scroll << std::endl;
    }

    Enable(); // Renable editor
}

void EditorFrame::OnAddLayerArt(wxCommandEvent &event)
{
    wxFileDialog fd(this, wxT("Add Background Layer..."), "", "", "PNG Files (*.png)|*.png", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (fd.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    long idx = layer_art_list->InsertItem(0, fd.GetPath());
    layer_art_list->SetItem(idx, 1, "0");

    layer_art_list->SetColumnWidth(0, wxLIST_AUTOSIZE);
}

void EditorFrame::OnLayerProps(wxCommandEvent &event)
{
    std::vector<int> items_to_edit;

    long item = -1;
    while (true)
    {
        item = layer_art_list->GetNextItem(item,
                                           wxLIST_NEXT_ALL,
                                           wxLIST_STATE_SELECTED);
        if (item == -1)
            break;

        items_to_edit.push_back(item);
    }

    if (items_to_edit.size() == 1)
    {
        int item_idx = items_to_edit[0];

        wxString file_path = layer_art_list->GetItemText(item_idx, 0);
        wxString scroll_speed = layer_art_list->GetItemText(item_idx, 1);

        Disable();
        lp = new LayerPropertiesDlg(this, file_path, scroll_speed, item_idx, layer_art_list->GetItemCount());
        lp->list_item_modified = item_idx;
        lp->Bind(wxEVT_CLOSE_WINDOW, &EditorFrame::OnLayerPropsClose, this);
        lp->Show();
    }
}

void EditorFrame::OnLayerPropsClose(wxCloseEvent &event)
{
    lp->Destroy();
    Enable();

    int new_location = lp->GetNewOrder();
    int old_location = lp->list_item_modified;

    wxString new_file = lp->GetNewPath();
    wxString scroll = lp->GetNewScroll();

    if (new_location != old_location)
    {
        layer_art_list->DeleteItem(old_location);
        layer_art_list->InsertItem(new_location, new_file);
        layer_art_list->SetItem(new_location, 1, scroll);
    }
    else
    {
        layer_art_list->SetItem(old_location, 0, new_file);
        layer_art_list->SetItem(old_location, 1, scroll);
    }

    layer_art_list->SetColumnWidth(0, wxLIST_AUTOSIZE);
}

std::vector<std::string> EditorFrame::GetLayers()
{
    Disable(); // Disable Editor while displaying testbackground

    std::vector<std::string> layer_filenames;

    // Extract layer information from editor
    long item = -1;
    while (true)
    {
        item = layer_art_list->GetNextItem(item);

        if (item == -1)
            break;

        std::string filename = layer_art_list->GetItemText(item).ToStdString();
        layer_filenames.push_back(filename);
    }

    Enable(); // Renable editor

    return layer_filenames;
}

LayerPropertiesDlg::LayerPropertiesDlg(wxWindow *parent, wxString layer_path, wxString layer_scroll, int item_num, int max_item)
    : wxFrame(parent, wxID_ANY, "Layer Properties")
{

    filepath = layer_path;

    layout = new wxGridSizer(2, 10, 10);
    this->SetSizer(layout);

    file_path_screentext = new wxStaticText(this, wxID_ANY, "Filename: " + filepath);
    layout->Add(file_path_screentext, 0, wxEXPAND | wxALL, 10);

    wxButton *change_filepath = new wxButton(this, wxID_ANY, "Change Filepath");
    change_filepath->Bind(wxEVT_BUTTON, &LayerPropertiesDlg::OnChangeFile, this);
    layout->Add(change_filepath, 0, wxEXPAND | wxALL, 10);

    wxStaticText *speed_text = new wxStaticText(this, wxID_ANY, "Adjust Scroll Speed");
    layout->Add(speed_text, 0, wxEXPAND | wxALL, 10);

    speed_ctrl = new wxSpinCtrlDouble(this, wxID_ANY, layer_scroll);
    layout->Add(speed_ctrl, 0, wxEXPAND | wxALL, 10);

    wxStaticText *layer_text = new wxStaticText(this, wxID_ANY, "Layer Number");
    layout->Add(layer_text, 0, wxEXPAND | wxALL, 10);

    order_ctrl = new wxSpinCtrl(this, wxID_ANY, std::to_string(item_num), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, max_item - 1);
    layout->Add(order_ctrl, 0, wxEXPAND | wxALL, 10);
}

void LayerPropertiesDlg::OnChangeFile(wxCommandEvent &event)
{
    wxFileDialog fd(this, wxT("Change Artwork..."), "", "", "PNG Files (*.png)|*.png", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (fd.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    this->file_path_screentext->SetLabel("Filename: " + fd.GetPath());
    this->layout->Layout();
    this->filepath = fd.GetPath();
}

wxString LayerPropertiesDlg::GetNewPath()
{
    return filepath;
}

wxString LayerPropertiesDlg::GetNewScroll()
{
    return std::to_string(speed_ctrl->GetValue());
}

int LayerPropertiesDlg::GetNewOrder()
{
    return order_ctrl->GetValue();
}

void LayerPropertiesDlg::OnExit(wxCommandEvent &event)
{
    Close(true);
}

void EditorFrame::OnRemoveLayer(wxCommandEvent &event)
{
    std::vector<int> items_to_remove;

    long item = -1;
    while (true)
    {
        item = layer_art_list->GetNextItem(item,
                                           wxLIST_NEXT_ALL,
                                           wxLIST_STATE_SELECTED);
        if (item == -1)
            break;

        items_to_remove.push_back(item);
    }

    if (items_to_remove.size() == layer_art_list->GetItemCount())
    {
        layer_art_list->DeleteAllItems();
        return;
    }

    for (int i : items_to_remove)
    {
        layer_art_list->DeleteItem(i);
    }
}

void EditorFrame::InitTilesPanel()
{
    tiles_panel = new wxPanel(tools, wxID_ANY);
    tiles_layout = new wxBoxSizer(wxVERTICAL);

    tiles_panel->SetSizer(tiles_layout);

    wxButton *add_tile_bttn = new wxButton(tiles_panel, wxID_ANY, "Add Tile");
    add_tile_bttn->Bind(wxEVT_BUTTON, &EditorFrame::OnAddTileArt, this);

    wxButton *rm_tile_bttn = new wxButton(tiles_panel, wxID_ANY, "Remove Selected Tile");
    rm_tile_bttn->Bind(wxEVT_BUTTON, &EditorFrame::OnRemoveTileArt, this);

    wxButton *tile_props_bttn = new wxButton(tiles_panel, wxID_ANY, "Edit Selected Tile Properties");
    tile_props_bttn->Bind(wxEVT_BUTTON, &EditorFrame::OnEditTileProps, this);

    wxButton *place_tiles = new wxButton(tiles_panel, wxID_ANY, "Place Tiles");
    place_tiles->Bind(wxEVT_BUTTON, &EditorFrame::OnPlaceTiles, this);

    tile_arts = new wxListCtrl(tiles_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES | wxLC_REPORT);
    tile_arts->InsertColumn(0, "Filename");
    tile_arts->InsertColumn(1, "Foreground Tile?");
    tile_arts->InsertColumn(2, "Collision Tile?");
    tile_arts->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
    tile_arts->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
    tile_arts->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);

    tiles_layout->Add(add_tile_bttn, 0, wxEXPAND | wxALL, 10);
    tiles_layout->Add(rm_tile_bttn, 0, wxEXPAND | wxALL, 10);
    tiles_layout->Add(tile_props_bttn, 0, wxEXPAND | wxALL, 10);
    tiles_layout->Add(place_tiles, 0, wxEXPAND | wxALL, 10);
    tiles_layout->Add(tile_arts, 0, wxEXPAND | wxALL, 10);
}

void EditorFrame::OnAddTileArt(wxCommandEvent &event)
{
    wxFileDialog fd(this, wxT("Add Tile..."), "", "", "PNG Files (*.png)|*.png", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (fd.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    long idx = tile_arts->InsertItem(0, fd.GetPath());
    tile_arts->SetItem(idx, 1, "True");
    tile_arts->SetItem(idx, 2, "True");
    tile_arts->SetColumnWidth(0, wxLIST_AUTOSIZE);
}

void EditorFrame::OnRemoveTileArt(wxCommandEvent &event)
{
    std::vector<int> items_to_remove;

    long item = -1;
    while (true)
    {
        item = tile_arts->GetNextItem(item,
                                      wxLIST_NEXT_ALL,
                                      wxLIST_STATE_SELECTED);
        if (item == -1)
            break;

        items_to_remove.push_back(item);
    }

    if (items_to_remove.size() == tile_arts->GetItemCount())
    {
        tile_arts->DeleteAllItems();
        return;
    }

    for (int i : items_to_remove)
    {
        tile_arts->DeleteItem(i);
    }
}

void EditorFrame::OnEditTileProps(wxCommandEvent &event)
{
    std::vector<int> items_to_edit;

    long item = -1;
    while (true)
    {
        item = tile_arts->GetNextItem(item,
                                      wxLIST_NEXT_ALL,
                                      wxLIST_STATE_SELECTED);
        if (item == -1)
            break;

        items_to_edit.push_back(item);
    }

    if (items_to_edit.size() == 1)
    {
        int item_idx = items_to_edit[0];

        wxString file_path = tile_arts->GetItemText(item_idx, 0);
        wxString isForeground = tile_arts->GetItemText(item_idx, 1);
        wxString doesCollide = tile_arts->GetItemText(item_idx, 2);

        Disable();
        tp = new TilePropertiesDlg(this, item_idx, file_path, isForeground, doesCollide);
        tp->Bind(wxEVT_CLOSE_WINDOW, &EditorFrame::OnTilePropsClose, this);
        tp->Show();
    }
}

void EditorFrame::OnTilePropsClose(wxCloseEvent &event)
{
    tp->Destroy();
    Enable();

    wxString new_file = tp->GetNewPath();
    wxString foreground = tp->GetNewForegroundStatus();
    wxString collision = tp->GetNewCollisionStatus();

    tile_arts->SetItem(tp->idx, 0, new_file);
    tile_arts->SetItem(tp->idx, 1, foreground);
    tile_arts->SetItem(tp->idx, 2, collision);

    tile_arts->SetColumnWidth(0, wxLIST_AUTOSIZE);
}

TilePropertiesDlg::TilePropertiesDlg(wxWindow *parent, int idx, wxString tile_path, wxString Foreground, wxString Collides)
    : wxFrame(parent, wxID_ANY, "Tile Properties")
{

    this->idx = idx;
    filepath = tile_path;

    layout = new wxGridSizer(2, 10, 10);
    this->SetSizer(layout);

    file_path_screentext = new wxStaticText(this, wxID_ANY, "Filename: " + tile_path);
    layout->Add(file_path_screentext, 0, wxEXPAND | wxALL, 10);

    wxButton *change_filepath = new wxButton(this, wxID_ANY, "Change Filepath");
    change_filepath->Bind(wxEVT_BUTTON, &TilePropertiesDlg::OnChangeFile, this);
    layout->Add(change_filepath, 0, wxEXPAND | wxALL, 10);

    wxStaticText *has_foreground_text = new wxStaticText(this, wxID_ANY, "Tile Foreground Status:");
    layout->Add(has_foreground_text, 0, wxEXPAND | wxALL, 10);

    foreground_box = new wxComboBox(this, wxID_ANY);
    foreground_box->Append("True");
    foreground_box->Append("False");
    foreground_box->SetSelection((Foreground == "True" ? 0 : 1));
    layout->Add(foreground_box, 0, wxEXPAND | wxALL, 10);

    wxStaticText *has_collision_text = new wxStaticText(this, wxID_ANY, "Tile Collision Status:");
    layout->Add(has_collision_text, 0, wxEXPAND | wxALL, 10);

    collision_box = new wxComboBox(this, wxID_ANY);
    collision_box->Append("True");
    collision_box->Append("False");
    collision_box->SetSelection((Collides == "True" ? 0 : 1));
    layout->Add(collision_box, 0, wxEXPAND | wxALL, 10);
}

void TilePropertiesDlg::OnChangeFile(wxCommandEvent &event)
{
    wxFileDialog fd(this, wxT("Change Artwork..."), "", "", "PNG Files (*.png)|*.png", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (fd.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    this->file_path_screentext->SetLabel("Filename: " + fd.GetPath());
    this->layout->Layout();
    this->filepath = fd.GetPath();
}

wxString TilePropertiesDlg::GetNewCollisionStatus()
{
    return this->collision_box->GetStringSelection();
}

wxString TilePropertiesDlg::GetNewForegroundStatus()
{
    return this->foreground_box->GetStringSelection();
}

wxString TilePropertiesDlg::GetNewPath()
{
    return this->filepath;
}

// TODO: Editor Team: Allow user to place selected tile
void EditorFrame::OnPlaceTiles(wxCommandEvent &event)
{
    Disable(); // Disable editor while placing tiles

    std::string current_tile_path = "";
    bool is_foreground_tile = false;
    bool is_collision_tile = false;

    std::vector<int> items_to_remove;

    long item = -1;
    while (true)
    {
        item = tile_arts->GetNextItem(item,
                                      wxLIST_NEXT_ALL,
                                      wxLIST_STATE_SELECTED);
        if (item == -1)
            break;

        items_to_remove.push_back(item);
    }

    if (items_to_remove.size() == 1)
    {
        item = items_to_remove.at(0);
        current_tile_path = tile_arts->GetItemText(item).ToStdString();

        std::string is_collision_tile_str = tile_arts->GetItemText(item, 1).ToStdString();
        is_collision_tile = is_collision_tile_str == "True" ? true : false;

        std::string is_foreground_tile_str = tile_arts->GetItemText(item, 2).ToStdString();
        is_foreground_tile = is_foreground_tile_str == "True" ? true : false;

        std::cout << current_tile_path << " (" << is_collision_tile << ", " << is_foreground_tile << ")\n";

        // TODO: Allow user to set tile locations... store that somewhere (IDK where, pick someplace)
        // TODO: Don't forget to load previously placed tiles
    }

    wxString spriteFile = sprites_list->GetItemText(0, 0).ToStdString();
    // TODO Game Engine Team: Allow the user to play the game!
    Engine *game = new Engine("Game", ScreenWidth, ScreenHeight);
    Sprite *sprite = new Sprite(spriteFile, game->renderer, 3, 12);

    // sprite->selectSprite(0, 0, 224);

    /*
    game->addLayer("./test_assets/m[1].png");
    game->addLayer("./test_assets/revised_pics/layer_3.png");
    game->addLayer("./test_assets/ed[2].png");
    game->addLayer("./test_assets/ed[0].png");
    game->addLayer("./test_assets/front_layer.png");
    */
    std::vector<std::string> layer_filenames;
    layer_filenames = EditorFrame::GetLayers();
    for (int i = 0; i < layer_filenames.size(); i++)
    {

        std::string filename = layer_filenames.at(i);

        game->addLayer(filename.c_str());
    }

    Image *image2 = game->getLayer(0); // Gets the first layer from the add layer vector
    game->addTiles("./test_assets/Dirt.png");
    game->addTiles("./test_assets/deepDirt.png");
    game->addTiles("./test_assets/Grass.png");
    game->changeTile(1);
    game->setGridSize(32);
    int gridSize = game->getGridSize();
    game->initializeTileMap(gridSize, ScreenWidth, ScreenHeight);

    Uint64 lastFrameTime = SDL_GetPerformanceCounter();
    SDL_Event happen;
    while (SDL_PollEvent(&happen) >= 0)
    {
        Uint64 currentFrameTime = SDL_GetPerformanceCounter();
        game->setDeltaTime((currentFrameTime - lastFrameTime) / static_cast<double>(SDL_GetPerformanceFrequency()));
        lastFrameTime = currentFrameTime;
        switch (happen.type)
        {
        case SDL_QUIT:
            SDL_Quit();
            return;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (happen.button.button == SDL_BUTTON_LEFT)
            {
                int mouseX = happen.button.x;
                int mouseY = happen.button.y;
                game->tilemap(gridSize, game->get_Screen_width(), game->get_Screen_height(), mouseX, mouseY);
            }
            if (happen.button.button == SDL_BUTTON_RIGHT)
            {
                game->changeTile(1);
            }
            break;

        case SDL_MOUSEMOTION:
            if (happen.button.button == SDL_BUTTON_LEFT)
            {
                int mouseX = happen.button.x;
                int mouseY = happen.button.y;
                game->tilemap(gridSize, game->get_Screen_width(), game->get_Screen_height(), mouseX, mouseY);
            }
            break;
        case SDL_KEYDOWN:
            switch (happen.key.keysym.sym)
            {
                // add scrolling here so when sprite moves forward the backround will scroll with it...
            case SDLK_d:
                game->moveRight(sprite, 15);
                game->set_backroundLocation(game->get_backroundLocation() + ScrollSpeed);
                game->set_backroundLocation2(game->get_backroundLocation2() + ScrollSpeed2);
                game->set_backroundLocation3(game->get_backroundLocation3() + 3);
                game->set_backroundLocation4(game->get_backroundLocation4() + 4);
                game->set_backroundLocation5(game->get_backroundLocation5() + 5);
                break;
                // same logic except for backwards...
            case SDLK_a:
                game->moveLeft(sprite, 15);
                game->set_backroundLocation(game->get_backroundLocation() - ScrollSpeed);
                game->set_backroundLocation2(game->get_backroundLocation2() - ScrollSpeed2);
                game->set_backroundLocation3(game->get_backroundLocation3() - 3);
                game->set_backroundLocation4(game->get_backroundLocation4() - 4);
                game->set_backroundLocation5(game->get_backroundLocation5() - 5);

                break;

            case SDLK_SPACE:
                game->jump(sprite, 10);
                break;

            case SDLK_4:
                game->respawn(sprite);
                break;
            case SDLK_r:
                game->respawn_x(sprite);
                break;
            case SDLK_f:
                game->spawn(sprite);
                break;
            }
        }
        // 320,180
        //  Clears the renderer, then copies the background and background copy to the render target, and then the foreground is copied.
        SDL_RenderClear(game->renderer);
        game->setRenderCopy(game->backgrounds, 0, 0, 1920, 1080, game->get_Screen_width(), game->get_Screen_height());
        // game->grid(gridSize, ScreenWidth, ScreenHeight);
        game->renderTileMap();
        sprite->drawSelectedSprite(game->renderer);
        game->resolveCollisions(sprite);
        game->applyGravity(sprite);
        sprite->deccelerate();
        sprite->setState();
        sprite->animate(0, 1, 2, 12);
        sprite->spriteJump();
        SDL_RenderPresent(game->renderer);
    }

    // Cleaning up all the variables once SDL_Quit() is called. Destroying the textures and surfaces, and then assigning all pointers used to nullptr
    game->window = nullptr;
    game->renderer = nullptr;
    image2 = nullptr;

    game->~Engine();

    Enable(); // Enable editor after done placing tiles
}

void EditorFrame::InitSpritesPanel()
{
    sprites_panel = new wxPanel(tools, wxID_ANY);
    sprites_layout = new wxBoxSizer(wxVERTICAL);

    sprites_panel->SetSizer(sprites_layout);

    wxButton *add_sprite_sheet = new wxButton(sprites_panel, wxID_ANY, "Add Sprite");
    add_sprite_sheet->Bind(wxEVT_BUTTON, &EditorFrame::OnAddSprite, this);

    wxButton *rm_sprite = new wxButton(sprites_panel, wxID_ANY, "Remove Sprite");
    rm_sprite->Bind(wxEVT_BUTTON, &EditorFrame::OnRemoveSprite, this);

    wxButton *sprite_props = new wxButton(sprites_panel, wxID_ANY, "Sprite Properties");
    sprite_props->Bind(wxEVT_BUTTON, &EditorFrame::OnSpriteProps, this);

    sprites_list = new wxListCtrl(sprites_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES | wxLC_REPORT);
    sprites_list->InsertColumn(0, "Sprite Sheet Filepath");
    sprites_list->InsertColumn(1, "Sprite Height");
    sprites_list->InsertColumn(2, "Sprite Width");
    sprites_list->InsertColumn(3, "User Controlled?");
    sprites_list->InsertColumn(4, "X Position");
    sprites_list->InsertColumn(5, "Y Position");
    sprites_list->InsertColumn(6, "Jump Height");
    sprites_list->InsertColumn(7, "Speed");
    sprites_list->InsertColumn(8, "Projectile?");
    sprites_list->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
    sprites_list->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
    sprites_list->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
    sprites_list->SetColumnWidth(3, wxLIST_AUTOSIZE_USEHEADER);
    sprites_list->SetColumnWidth(4, wxLIST_AUTOSIZE_USEHEADER);
    sprites_list->SetColumnWidth(5, wxLIST_AUTOSIZE_USEHEADER);
    sprites_list->SetColumnWidth(6, wxLIST_AUTOSIZE_USEHEADER);
    sprites_list->SetColumnWidth(7, wxLIST_AUTOSIZE_USEHEADER);
    sprites_list->SetColumnWidth(8, wxLIST_AUTOSIZE_USEHEADER);

    sprites_layout->Add(add_sprite_sheet, 0, wxEXPAND | wxALL, 10);
    sprites_layout->Add(rm_sprite, 0, wxEXPAND | wxALL, 10);
    sprites_layout->Add(sprite_props, 0, wxEXPAND | wxALL, 10);
    sprites_layout->Add(sprites_list, 0, wxEXPAND | wxALL, 10);
}

void EditorFrame::OnAddSprite(wxCommandEvent &event)
{
    wxFileDialog fd(this, wxT("Select Sprite Sheet..."), "", "", "PNG Files (*.png)|*.png", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (fd.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    long idx = sprites_list->InsertItem(0, fd.GetPath());
    sprites_list->SetItem(idx, 1, "0");
    sprites_list->SetItem(idx, 2, "0");
    sprites_list->SetItem(idx, 3, "False");
    sprites_list->SetItem(idx, 4, "0");
    sprites_list->SetItem(idx, 5, "0");
    sprites_list->SetItem(idx, 6, "0");
    sprites_list->SetItem(idx, 7, "0");
    sprites_list->SetItem(idx, 8, "False");

    sprites_list->SetColumnWidth(0, wxLIST_AUTOSIZE);
}

void EditorFrame::OnRemoveSprite(wxCommandEvent &event)
{
    std::vector<int> items_to_remove;

    long item = -1;
    while (true)
    {
        item = sprites_list->GetNextItem(item,
                                         wxLIST_NEXT_ALL,
                                         wxLIST_STATE_SELECTED);
        if (item == -1)
            break;

        items_to_remove.push_back(item);
    }

    if (items_to_remove.size() == sprites_list->GetItemCount())
    {
        sprites_list->DeleteAllItems();
        return;
    }

    for (int i : items_to_remove)
    {
        sprites_list->DeleteItem(i);
    }
}

void EditorFrame::OnSpriteProps(wxCommandEvent &event)
{
    std::vector<int> items_to_edit;

    long item = -1;
    while (true)
    {
        item = sprites_list->GetNextItem(item,
                                         wxLIST_NEXT_ALL,
                                         wxLIST_STATE_SELECTED);
        if (item == -1)
            break;

        items_to_edit.push_back(item);
    }

    if (items_to_edit.size() == 1)
    {
        int item_idx = items_to_edit[0];

        wxString file_path = sprites_list->GetItemText(item_idx, 0);
        wxString height = sprites_list->GetItemText(item_idx, 1);
        wxString width = sprites_list->GetItemText(item_idx, 2);
        wxString x = sprites_list->GetItemText(item_idx, 3);
        wxString y = sprites_list->GetItemText(item_idx, 4);
        wxString user_ctrl = sprites_list->GetItemText(item_idx, 5);
        wxString jumph = sprites_list->GetItemText(item_idx, 6);
        wxString speed = sprites_list->GetItemText(item_idx, 7);
        wxString proj = sprites_list->GetItemText(item_idx, 8);

        Disable();
        sp = new SpritePropertiesDlg(this, item_idx, file_path, height, width, x, y, user_ctrl, jumph, speed, proj);
        sp->Bind(wxEVT_CLOSE_WINDOW, &EditorFrame::OnSpritePropsClose, this);
        sp->Show();
    }
}

SpritePropertiesDlg::SpritePropertiesDlg(wxWindow *parent, int idx, wxString file, wxString height_in, wxString width_in, wxString user_control, wxString x_in, wxString y_in, wxString jump_in, wxString speed_in, wxString proj_in)
    : wxFrame(parent, wxID_ANY, "Sprite Properties")
{

    filepath = file;
    this->list_index = idx;

    layout = new wxGridSizer(2, 10, 10);
    this->SetSizer(layout);

    file_path_screentext = new wxStaticText(this, wxID_ANY, "Filename: " + file);
    layout->Add(file_path_screentext, 0, wxEXPAND | wxALL, 10);

    wxButton *change_filepath = new wxButton(this, wxID_ANY, "Change Filepath");
    change_filepath->Bind(wxEVT_BUTTON, &SpritePropertiesDlg::OnChangeFile, this);
    layout->Add(change_filepath, 0, wxEXPAND | wxALL, 10);

    wxStaticText *height_text = new wxStaticText(this, wxID_ANY, "Sprite Height");
    layout->Add(height_text, 0, wxEXPAND | wxALL, 10);

    height = new wxSpinCtrl(this, wxID_ANY, height_in);
    height->SetRange(0, 999);
    layout->Add(height, 0, wxEXPAND | wxALL, 10);

    wxStaticText *width_text = new wxStaticText(this, wxID_ANY, "Sprite Width");
    layout->Add(width_text, 0, wxEXPAND | wxALL, 10);

    width = new wxSpinCtrl(this, wxID_ANY, width_in);
    width->SetRange(0, 999);
    layout->Add(width, 0, wxEXPAND | wxALL, 10);

    wxStaticText *x_text = new wxStaticText(this, wxID_ANY, "X coordinate");
    layout->Add(x_text, 0, wxEXPAND | wxALL, 10);

    x = new wxSpinCtrl(this, wxID_ANY, x_in);
    x->SetRange(0, 999);
    layout->Add(x, 0, wxEXPAND | wxALL, 10);

    wxStaticText *y_text = new wxStaticText(this, wxID_ANY, "Y coordinate");
    layout->Add(y_text, 0, wxEXPAND | wxALL, 10);

    y = new wxSpinCtrl(this, wxID_ANY, y_in);
    y->SetRange(0, 999);
    layout->Add(y, 0, wxEXPAND | wxALL, 10);

    wxStaticText *user_ctrl_text = new wxStaticText(this, wxID_ANY, "User Controlled Sprite?");
    layout->Add(user_ctrl_text, 0, wxEXPAND | wxALL, 10);

    user_ctrl = new wxComboBox(this, wxID_ANY);
    user_ctrl->Append("True");
    user_ctrl->Append("False");
    user_ctrl->SetSelection((user_control == "True" ? 0 : 1));
    layout->Add(user_ctrl, 0, wxEXPAND | wxALL, 10);

    wxStaticText *jump_text = new wxStaticText(this, wxID_ANY, "Jump Height");
    layout->Add(jump_text, 0, wxEXPAND | wxALL, 10);

    jumph = new wxSpinCtrl(this, wxID_ANY, jump_in);
    jumph->SetRange(0, 999);
    layout->Add(jumph, 0, wxEXPAND | wxALL, 10);

    wxStaticText *speed_text = new wxStaticText(this, wxID_ANY, "Sprite Speed");
    layout->Add(speed_text, 0, wxEXPAND | wxALL, 10);

    speed = new wxSpinCtrl(this, wxID_ANY, speed_in);
    speed->SetRange(0, 999);
    layout->Add(speed, 0, wxEXPAND | wxALL, 10);

    wxStaticText *proj_text = new wxStaticText(this, wxID_ANY, "Projectile?");
    layout->Add(proj_text, 0, wxEXPAND | wxALL, 10);

    proj = new wxComboBox(this, wxID_ANY);
    proj->Append("True");
    proj->Append("False");
    proj->SetSelection((proj_in == "True" ? 0 : 1));
    layout->Add(proj, 0, wxEXPAND | wxALL, 10);
}

void SpritePropertiesDlg::OnChangeFile(wxCommandEvent &event)
{
    wxFileDialog fd(this, wxT("Change Artwork..."), "", "", "PNG Files (*.png)|*.png", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (fd.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    this->file_path_screentext->SetLabel("Filename: " + fd.GetPath());
    this->layout->Layout();
    this->filepath = fd.GetPath();
}

wxString SpritePropertiesDlg::GetNewPath()
{
    return this->filepath;
}

wxString SpritePropertiesDlg::GetNewHeight()
{
    return std::to_string(height->GetValue());
}

wxString SpritePropertiesDlg::GetNewWidth()
{
    return std::to_string(width->GetValue());
}

wxString SpritePropertiesDlg::GetNewUserControlStatus()
{
    return user_ctrl->GetStringSelection();
}

wxString SpritePropertiesDlg::GetSpriteX()
{
    return std::to_string(x->GetValue());
}

wxString SpritePropertiesDlg::GetSpriteY()
{
    return std::to_string(y->GetValue());
}

wxString SpritePropertiesDlg::GetSpriteJump()
{
    return std::to_string(jumph->GetValue());
}

wxString SpritePropertiesDlg::GetSpriteSpeed()
{
    return std::to_string(speed->GetValue());
}

wxString SpritePropertiesDlg::GetProjectile()
{
    return proj->GetStringSelection();
}

void EditorFrame::OnSpritePropsClose(wxCloseEvent &event)
{
    sp->Destroy();
    Enable();

    wxString new_file = sp->GetNewPath();
    wxString height = sp->GetNewHeight();
    wxString width = sp->GetNewWidth();
    wxString user_ctrl = sp->GetNewUserControlStatus();
    wxString x = sp->GetSpriteX();
    wxString y = sp->GetSpriteY();
    wxString jumph = sp->GetSpriteJump();
    wxString speed = sp->GetSpriteSpeed();
    wxString proj = sp->GetProjectile();

    sprites_list->SetItem(sp->list_index, 0, new_file);
    sprites_list->SetItem(sp->list_index, 1, height);
    sprites_list->SetItem(sp->list_index, 2, width);
    sprites_list->SetItem(sp->list_index, 3, user_ctrl);
    sprites_list->SetItem(sp->list_index, 4, x);
    sprites_list->SetItem(sp->list_index, 5, y);
    sprites_list->SetItem(sp->list_index, 6, jumph);
    sprites_list->SetItem(sp->list_index, 7, speed);
    sprites_list->SetItem(sp->list_index, 8, proj);

    sprites_list->SetColumnWidth(0, wxLIST_AUTOSIZE);
}

// TODO: Engine Team: Allow user to place sprites in game
void EditorFrame::OnPlaceSprites(wxCommandEvent &event)
{
    Disable();

    Enable();
}

int_fast8_t EditorFrame::getSprite()
{

    long item = -1;
    std::vector<int> items_to_edit;
    while (true)
    {
        item = sprites_list->GetNextItem(item,
                                         wxLIST_NEXT_ALL,
                                         wxLIST_STATE_SELECTED);
        if (item == -1)
            break;

        items_to_edit.push_back(item);
    }

    if (items_to_edit.size() == 1)
    {
        item = items_to_edit.at(0);
        return item;
    }

    return -1;
}

void EditorFrame::OnNew(wxCommandEvent &event)
{
    std::cout << "TODO: New Game" << std::endl;
}

void EditorFrame::OnSave(wxCommandEvent &event)
{
    std::cout << "TODO: Save Game" << std::endl;
}

void EditorFrame::OnOpen(wxCommandEvent &event)
{
    std::cout << "TODO: Open Game" << std::endl;
}

void EditorFrame::OnPlay(wxCommandEvent &event)
{
    // wxString spritefile = SpritePropertiesDlg::GetNewPath();
    wxString spriteFile = sprites_list->GetItemText(0, 0).ToStdString();
    // TODO Game Engine Team: Allow the user to play the game!
    Engine *game = new Engine("Game", ScreenWidth, ScreenHeight);
    Sprite *sprite = new Sprite(spriteFile, game->renderer, 3, 12);

    // sprite->selectSprite(0, 0, 224);

    /*
    game->addLayer("./test_assets/m[1].png");
    game->addLayer("./test_assets/revised_pics/layer_3.png");
    game->addLayer("./test_assets/ed[2].png");
    game->addLayer("./test_assets/ed[0].png");
    game->addLayer("./test_assets/front_layer.png");
    */
    std::vector<std::string> layer_filenames;
    layer_filenames = EditorFrame::GetLayers();
    for (int i = 0; i < layer_filenames.size(); i++)
    {

        std::string filename = layer_filenames.at(i);

        game->addLayer(filename.c_str());
    }

    Image *image2 = game->getLayer(0); // Gets the first layer from the add layer vector
    game->addTiles("./test_assets/Dirt.png");
    game->addTiles("./test_assets/deepDirt.png");
    game->addTiles("./test_assets/Grass.png");
    game->changeTile(1);

    game->setGridSize(32);
    int gridSize = game->getGridSize();
    game->initializeTileMap(gridSize, ScreenWidth, ScreenHeight);

    Uint64 lastFrameTime = SDL_GetPerformanceCounter();
    SDL_Event happen;
    while (SDL_PollEvent(&happen) >= 0)
    {
        Uint64 currentFrameTime = SDL_GetPerformanceCounter();
        game->setDeltaTime((currentFrameTime - lastFrameTime) / static_cast<double>(SDL_GetPerformanceFrequency()));
        lastFrameTime = currentFrameTime;
        switch (happen.type)
        {
        case SDL_QUIT:
            SDL_Quit();
            game->save();
            return;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (happen.button.button == SDL_BUTTON_LEFT)
            {
                int mouseX = happen.button.x;
                int mouseY = happen.button.y;
                game->tilemap(gridSize, game->get_Screen_width(), game->get_Screen_height(), mouseX, mouseY);
            }
            if (happen.button.button == SDL_BUTTON_RIGHT)
            {
                game->changeTile(1);
            }
            break;

        case SDL_MOUSEMOTION:
            if (happen.button.button == SDL_BUTTON_LEFT)
            {
                int mouseX = happen.button.x;
                int mouseY = happen.button.y;
                game->tilemap(gridSize, game->get_Screen_width(), game->get_Screen_height(), mouseX, mouseY);
            }
            break;
        case SDL_KEYDOWN:
            switch (happen.key.keysym.sym)
            {
                // add scrolling here so when sprite moves forward the backround will scroll with it...
            case SDLK_d:
                game->moveRight(sprite, 15);
                game->set_backroundLocation(game->get_backroundLocation() + ScrollSpeed);
                game->set_backroundLocation2(game->get_backroundLocation2() + ScrollSpeed2);
                game->set_backroundLocation3(game->get_backroundLocation3() + 3);
                game->set_backroundLocation4(game->get_backroundLocation4() + 4);
                game->set_backroundLocation5(game->get_backroundLocation5() + 5);
                break;
                // same logic except for backwards...
            case SDLK_a:
                game->moveLeft(sprite, 15);
                game->set_backroundLocation(game->get_backroundLocation() - ScrollSpeed);
                game->set_backroundLocation2(game->get_backroundLocation2() - ScrollSpeed2);
                game->set_backroundLocation3(game->get_backroundLocation3() - 3);
                game->set_backroundLocation4(game->get_backroundLocation4() - 4);
                game->set_backroundLocation5(game->get_backroundLocation5() - 5);

                break;

            case SDLK_SPACE:
                game->jump(sprite, 10);
                break;

            case SDLK_4:
                game->respawn(sprite);
                break;
            case SDLK_r:
                game->respawn_x(sprite);
                break;
            case SDLK_f:
                game->spawn(sprite);
                break;
            }
        }
        // 320,180
        //  Clears the renderer, then copies the background and background copy to the render target, and then the foreground is copied.
        SDL_RenderClear(game->renderer);
        game->setRenderCopy(game->backgrounds, 0, 0, 1920, 1080, game->get_Screen_width(), game->get_Screen_height());
        // game->grid(gridSize, ScreenWidth, ScreenHeight);
        game->renderTileMap();
        sprite->drawSelectedSprite(game->renderer);
        game->resolveCollisions(sprite);
        game->applyGravity(sprite);
        sprite->deccelerate();
        sprite->setState();
        sprite->animate(0, 1, 2, 12);
        sprite->spriteJump();
        SDL_RenderPresent(game->renderer);
    }

    // Cleaning up all the variables once SDL_Quit() is called. Destroying the textures and surfaces, and then assigning all pointers used to nullptr
    game->window = nullptr;
    game->renderer = nullptr;
    image2 = nullptr;

    game->~Engine();
}

void EditorFrame::OnExit(wxCommandEvent &event)
{
    Close(true);
}

void EditorFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox("Fill in about dialogue here",
                 "About Project Otter", wxOK | wxICON_INFORMATION);
}