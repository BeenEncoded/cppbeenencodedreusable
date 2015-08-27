#include <wx/listctrl.h>
#include <stdexcept>
#include <exception>
#include <string>

#include "custom_list_control.hpp"
#include "data/flash_card.hpp"

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
                        list{},
                        str_display{nullptr},
                        selected{0}
{
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
wxString CustomListControl<type>::OnGetItemText(long x, const long column __attribute__((unused))) const
{
    if(!this->list.empty() && ((unsigned)x < this->list.size())) return this->str_display(this->list[x]);
    throw std::runtime_error{"Index " + std::to_string(x) + ": Out of bounds!"};
}

template<typename type>
void CustomListControl<type>::update_size()
{
    this->SetItemCount(this->list.size());
}

template<typename type>
bool CustomListControl<type>::item_selected() const
{
    return ((this->GetSelectedItemCount() > 0) && !this->list.empty() && (this->selected < this->list.size()));
}

/**
 * @brief Throws a runtime error if nothing is selected.
 */
template<typename type>
type& CustomListControl<type>::get_selected()
{
    if(this->item_selected()) return this->list[this->selected];
    else throw std::runtime_error{"Nothing Selected!"};
    return this->list[this->selected];
}

/**
 * @return true if a selected item was removed.
 */
template<typename type>
bool CustomListControl<type>::remove_selected()
{
    if(this->item_selected())
    {
        this->list.erase((this->list.begin() + this->selected));
        this->update_size();
        return true;
    }
    return false;
}

template class CustomListControl<data::deck_data>;
template class CustomListControl<data::flash_card_data>;

