#ifndef CUSTOM_LIST_CONTROL_HPP_INCLUDED
#define CUSTOM_LIST_CONTROL_HPP_INCLUDED

#include <wx/listctrl.h>
#include <wx/wx.h>
#include <string>
#include <vector>


template<typename type>
class CustomListControl : public wxListCtrl
{
public:
    CustomListControl(wxWindow*,
                wxWindowID = wxID_ANY,
                const wxPoint& = wxDefaultPosition,
                const wxSize& = wxDefaultSize,
                long = wxLC_ICON,
                const wxValidator& = wxDefaultValidator,
                const wxString& = wxListCtrlNameStr);
    
    wxString OnGetItemText(long, const long) const;
    void update_size();
    bool item_selected() const;
    type& get_selected();
    bool remove_selected();
    
    std::vector<type> list;
    std::string (*str_display)(const type&);
    std::size_t selected;
    
private:
    
};

#endif