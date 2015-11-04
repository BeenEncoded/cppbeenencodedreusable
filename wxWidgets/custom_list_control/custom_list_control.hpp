#ifndef CUSTOM_LIST_CONTROL_HPP_INCLUDED
#define CUSTOM_LIST_CONTROL_HPP_INCLUDED

#include <wx/listctrl.h>
#include <wx/wx.h>
#include <string>
#include <vector>
#include <functional>

/**
 * @class CustomListControl
 * @author Jonathan Whitlock
 * @date 09/30/2015
 * @file custom_list_control.hpp
 * @brief A base class for SingleListControl and MultiListControl.
 */
template<typename type>
class CustomListControl : public wxListCtrl
{
private:
    explicit CustomListControl() = default;
public:
    CustomListControl(wxWindow*,
                wxWindowID = wxID_ANY,
                const wxPoint& = wxDefaultPosition,
                const wxSize& = wxDefaultSize,
                long = wxLC_ICON,
                const wxValidator& = wxDefaultValidator,
                const wxString& = wxListCtrlNameStr);
    
    virtual ~CustomListControl();
    
    void set(std::vector<type>&, std::string (*)(const type&));
    
    wxString OnGetItemText(long, const long) const;
    
    bool item_selected() const;
    bool remove_selected();
    void push_back(const type&);
    void push_back(type&&);
    
    std::function<void()> selection_cb, deselection_cb, activation_cb;
    
protected:
    std::vector<type> *list;
    std::string (*str_display)(const type&);
    std::vector<std::size_t> selection;
    
    void update_size();
    void update_selection();
    virtual void init_control();
    void on_item_selected(wxListEvent&);
    void on_item_activation(wxListEvent&);
    void on_item_deselection(wxListEvent&);
    
};

/**
 * @class SingleListControl
 * @author Jonathan Whitlock
 * @date 10/13/2015
 * @file custom_list_control.hpp
 * @brief Applies the attributes of a Single-selection virtual list control.
 */
template<typename type>
class SingleListControl : public CustomListControl<type>
{
private:
    explicit SingleListControl() = default;
public:
    SingleListControl(wxWindow*,
                wxWindowID = wxID_ANY,
                const wxPoint& = wxDefaultPosition,
                const wxSize& = wxDefaultSize,
                long = wxLC_ICON,
                const wxValidator& = wxDefaultValidator,
                const wxString& = wxListCtrlNameStr);
    ~SingleListControl();
    
    type& get_selected();
private:
    void init_control();
    
};

/**
 * @class SingleListControl
 * @author Jonathan Whitlock
 * @date 10/13/2015
 * @file custom_list_control.hpp
 * @brief Applies the attributes of a Multi-selection virtual list control.
 */
template<typename type>
class MultiListControl : public CustomListControl<type>
{
private:
    explicit MultiListControl() = default;
public:
    MultiListControl(wxWindow*,
                wxWindowID = wxID_ANY,
                const wxPoint& = wxDefaultPosition,
                const wxSize& = wxDefaultSize,
                long = wxLC_ICON,
                const wxValidator& = wxDefaultValidator,
                const wxString& = wxListCtrlNameStr);
    ~MultiListControl();
    
    std::vector<type*> get_selected();
private:
    void init_control();
    
};

#endif