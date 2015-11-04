#include <wx/listctrl.h>
#include <stdexcept>
#include <exception>
#include <string>
#include <utility>
#include <cmath>
#include <algorithm>
#include <vector>

#include "custom_list_control.hpp"

namespace
{
}

template<typename type>
CustomListControl<type>::CustomListControl(wxWindow *parent,
                wxWindowID winid,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxValidator& validator,
                const wxString& name) : 
                        wxListCtrl{parent, winid, pos, size, style, validator, name},
                        selection_cb{[]()->void{}},
                        deselection_cb{[]()->void{}},
                        activation_cb{[]()->void{}},
                        list{},
                        str_display{nullptr},
                        selection{}
{
    this->Bind(wxEVT_LIST_ITEM_SELECTED, &CustomListControl<type>::on_item_selected, this);
    this->Bind(wxEVT_LIST_ITEM_ACTIVATED, &CustomListControl<type>::on_item_activation, this);
    this->Bind(wxEVT_LIST_ITEM_DESELECTED, &CustomListControl<type>::on_item_deselection, this);
    
    if(this->GetColumnCount() == 0)
    {
        this->InsertColumn(0, "");
        
        {
            wxListItem col;
            this->GetColumn(0, col);
            col.SetWidth(this->GetSize().GetWidth());
            this->SetColumn(0, col);
        }
    }
}

template<typename type>
CustomListControl<type>::~CustomListControl()
{
    this->Unbind(wxEVT_LIST_ITEM_SELECTED, &CustomListControl<type>::on_item_selected, this);
    this->Unbind(wxEVT_LIST_ITEM_ACTIVATED, &CustomListControl<type>::on_item_activation, this);
    this->Unbind(wxEVT_LIST_ITEM_DESELECTED, &CustomListControl<type>::on_item_deselection, this);
    
    this->selection.erase(this->selection.begin(), this->selection.end());
}

template<typename type>
void CustomListControl<type>::set(std::vector<type>& v, std::string (*tostr)(const type&))
{
    this->list = &v;
    this->str_display = tostr;
    this->update_size();
    this->selection.erase(this->selection.begin(), this->selection.end());
}

template<typename type>
void CustomListControl<type>::init_control()
{
    throw std::runtime_error{"You must use a derived class!"};
}

template<typename type>
void CustomListControl<type>::push_back(const type& t)
{
    this->list->push_back(t);
    this->update_size();
}

template<typename type>
void CustomListControl<type>::push_back(type&& t)
{
    this->list->push_back(std::move(t));
    this->update_size();
}

/**
 * @brief Updates the controls list of selected indecies.
 */
template<typename type>
void CustomListControl<type>::update_selection()
{
    this->selection.erase(this->selection.begin(), this->selection.end());
    for(int x{0}; x < this->GetItemCount(); ++x)
    {
        if(this->GetItemState(x, wxLIST_STATE_SELECTED) != 0)
        {
            this->selection.push_back(x);
        }
    }
}

template<typename type>
void CustomListControl<type>::on_item_selected(wxListEvent& e __attribute__((unused)))
{
    this->update_selection();
    this->selection_cb();
}

template<typename type>
void CustomListControl<type>::on_item_deselection(wxListEvent& e __attribute__((unused)))
{
    this->update_selection();
    this->deselection_cb();
}

template<typename type>
void CustomListControl<type>::on_item_activation(wxListEvent& e __attribute__((unused)))
{
    this->update_selection();
    this->activation_cb();
}

template<typename type>
wxString CustomListControl<type>::OnGetItemText(long x, const long column __attribute__((unused))) const
{
    if(!this->list->empty() && ((unsigned)x < this->list->size())) return this->str_display(this->list->operator[](x));
    throw std::runtime_error{"Index " + std::to_string(x) + ": Out of bounds!"};
}

template<typename type>
void CustomListControl<type>::update_size()
{
    this->SetItemCount(this->list->size());
}

/**
 * @return true if anything is selected.  This also calls update_selection.
 */
template<typename type>
bool CustomListControl<type>::item_selected() const
{
    this->update_selection();
    //check the validity of the selection
    for(std::vector<std::size_t>::const_iterator it{this->selection.begin()}; it != this->selection.end(); ++it)
    {
        if(*it >= this->list->size()) return false;
    }
    return ((this->GetSelectedItemCount() > 0) && !this->list->empty() && !this->selection.empty());
}

/**
 * @return true if a selected item was removed.
 */
template<typename type>
bool CustomListControl<type>::remove_selected()
{
    constexpr const char* const funct_name{"bool CustomListControl<type>::remove_selected()"};
    
    if((this->GetWindowStyle() & wxLC_SINGLE_SEL) == wxLC_SINGLE_SEL)
    {
        if(this->item_selected())
        {
            this->list->erase((this->list->begin() + this->selection.front()));
            this->update_size();
            return true;
        }
    }
    else
    {
        if(this->item_selected())
        {
            for(std::vector<std::size_t>::iterator it{this->selection.begin()}; it != this->selection.end();)
            {
                if(*it < this->list->size())
                {
                    this->list->erase((this->list->begin() + (*it)));
                    it = this->selection.erase(it);
                    continue;
                }
                ++it;
            }
            if(this->selection.empty()) return true;
            else throw std::runtime_error{std::string{funct_name} + 
                    "   Error: Some of the selected items did not exist!"};
        }
    }
    return false;
}

//SingleListControl:
template<typename type>
SingleListControl<type>::SingleListControl(wxWindow* parent,
                wxWindowID winid,
                const wxPoint& pos,
                const wxSize& size,
                long icon,
                const wxValidator& validator,
                const wxString& name) : 
    CustomListControl<type>{parent, winid, pos, size, icon, validator, name}
{
    this->init_control();
}

template<typename type>
SingleListControl<type>::~SingleListControl()
{
}

template<typename type>
void SingleListControl<type>::init_control()
{
    long style{std::move(this->GetWindowStyle())};
    style |= (wxLC_LIST | wxLC_REPORT | wxLC_VIRTUAL | wxLC_NO_HEADER | 
            wxLC_SINGLE_SEL | wxLC_HRULES);
    this->SetWindowStyle(style);
}

/**
 * @brief Throws a runtime error if nothing is selected.
 */
template<typename type>
type& SingleListControl<type>::get_selected()
{
    this->update_selection();
    if(this->selection.empty()) this->selection.emplace_back(0);
    if(this->item_selected()) return (*(this->list))[this->selection.front()];
    else throw std::runtime_error{"Nothing Selected!"};
    return (*(this->list))[this->selection.front()];
}

//MultListControl
template<typename type>
MultiListControl<type>::MultiListControl(wxWindow* par,
                wxWindowID winid,
                const wxPoint& pos,
                const wxSize& size,
                long icon,
                const wxValidator& val,
                const wxString& name) : 
    CustomListControl<type>{par, winid, pos, size, icon, val, name}
{
    this->init_control();
}

template<typename type>
MultiListControl<type>::~MultiListControl()
{
}

template<typename type>
void MultiListControl<type>::init_control()
{
    long style{this->GetWindowStyle()};
    style |= (wxLC_LIST | wxLC_REPORT | wxLC_VIRTUAL | wxLC_NO_HEADER | 
            wxLC_HRULES);
    if((style & wxLC_SINGLE_SEL) == wxLC_SINGLE_SEL)
    {
        long tempi{0};
        for(unsigned int x{0}; x < sizeof(long); ++x)
        {
            if(std::pow(2, x) != wxLC_SINGLE_SEL)
            {
                tempi |= (long)std::pow(2, x);
            }
        }
        style &= tempi;
    }
    this->SetWindowStyle(style);
}

template<typename type>
std::vector<type*> MultiListControl<type>::get_selected()
{
    std::vector<type*> sel;
    
    this->update_selection();
    for(std::size_t x{0}; x < this->selection.size(); ++x)
    {
        if(this->selection[x] < this->list->size())
        {
            sel.emplace_back(&(this->list->operator[](this->selection[x])));
        }
    }
    return sel;
}

