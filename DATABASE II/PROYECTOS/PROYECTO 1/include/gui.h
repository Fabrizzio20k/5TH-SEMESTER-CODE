#ifndef GUI_H
#define GUI_H

#include <wx/wx.h>
#include <wx/grid.h>
#include "parser.h"

using namespace std;

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
    void OnSearch(wxCommandEvent& event);

private:
    wxTextCtrl* sqlQuery;
    wxStaticText* searchText;
    wxGrid* grid;
    wxDECLARE_EVENT_TABLE();
    SQLParser parser;
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_BUTTON(1001, MyFrame::OnSearch)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    MyFrame *frame = new MyFrame("Escriba su consulta SQL", wxDefaultPosition, wxSize(450, 340));
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
        : wxFrame(NULL, wxID_ANY, title, pos, size) {
    new wxStaticText(this, wxID_ANY, "Consulta SQL:", wxPoint(10, 10));
    sqlQuery = new wxTextCtrl(this, wxID_ANY, "", wxPoint(10, 30), wxSize(300, 30));
    new wxButton(this, 1001, "Confirmar busqueda", wxPoint(320, 30), wxSize(120, 30));

    grid = new wxGrid(this, -1, wxPoint(10, 70), wxSize(420, 400));
    grid->CreateGrid(10000, 4);
    grid->SetColLabelValue(0, "Codigo");
    grid->SetColLabelValue(1, "Nombre");
    grid->SetColLabelValue(2, "Apellido");
    grid->SetColLabelValue(3, "Edad");
    grid->SetColLabelValue(4, "Ciclo");

    searchText = new wxStaticText(this, wxID_ANY, "", wxPoint(10, 480));

    this->Fit();
}


void MyFrame::OnSearch(wxCommandEvent& event) {
    wxString query = sqlQuery->GetValue();
    string queryStr = string(query.mb_str());
    string message;
    parser.parse(queryStr);
    Command cmd = parser.getCommand();

    switch (cmd.type) {
        case CommandType::CREATE:
            searchText->SetForegroundColour(*wxGREEN);
            message = "Creando tabla " + cmd.table + " con indice: " + cmd.indexColumn + " y tipo: " + cmd.indexType;
            break;
        case CommandType::SELECT:
            searchText->SetForegroundColour(*wxBLUE);
            message = "Seleccionando de tabla " + cmd.table + " donde " + cmd.indexColumn + " = " + cmd.condition;
            break;
        case CommandType::RANGE:
            searchText->SetForegroundColour(*wxBLUE);
            message = "Seleccionando de tabla " + cmd.table + " donde " + cmd.indexColumn + " entre " + cmd.r1 + " y " + cmd.r2;
            break;
        case CommandType::INSERTAR:
            searchText->SetForegroundColour(*wxGREEN);
            message = "Insertando en tabla " + cmd.table + " valores (";
            for (int i = 0; i < cmd.values.size(); i++) {
                message += cmd.values[i];
                if (i < cmd.values.size() - 1) {
                    message += ", ";
                }
            }
            message += ")";
            break;
        case CommandType::BORRAR:
            searchText->SetForegroundColour(*wxRED);
            message = "Borrando de tabla " + cmd.table + " donde " + cmd.indexColumn + " = " + cmd.condition;
            break;
        default:
            searchText->SetForegroundColour(*wxRED);
            message = "Comando no reconocido";
    }
    searchText->SetLabel(message);
}

#endif
