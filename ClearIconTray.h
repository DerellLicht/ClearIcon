//  config.cpp
LRESULT save_cfg_file(void);
LRESULT read_config_file(void);

//  ClearIcon.cpp
extern unsigned fg_attr ;
void reset_icon_colors(bool my_select_color);

//  systray.cpp
void load_tray_menu(void);
void attach_tray_icon(HWND hwnd, char const * const szClassName);
BOOL respond_to_tray_clicks(HWND hwnd, LPARAM lParam);
void release_systray_resource(void);

//  about.cpp
BOOL CmdAbout(HWND hwnd);

