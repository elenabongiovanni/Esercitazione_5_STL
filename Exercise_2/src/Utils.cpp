#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

namespace PolygonalLib{

bool ImportCell0D(const string &filename, PolygonalMesh& mesh){
    ifstream file;
    file.open(filename);
    if(file.fail())
        return false;

    list<string> lines;
    string line;
    while(getline(file,line)){
        replace(line.begin(), line.end(),';', ' ');
        lines.push_back(line);
    }
    lines.pop_front();
    file.close();

    mesh.NumberCell0D = lines.size();
    if(mesh.NumberCell0D == 0){
        cerr<<"There is no cell 0D" << endl;
        return false;
    }
    mesh.IdCell0D.reserve(mesh.NumberCell0D);
    mesh.CoordinatesCell0D.reserve(mesh.NumberCell0D);

    for(const string& line:lines){
        istringstream convert(line);
        unsigned int id;
        unsigned int marker;
        Vector2d coords;
        convert >> id  >> marker  >> coords(0) >> coords(1);
        mesh.IdCell0D.push_back(id);
        mesh.CoordinatesCell0D.push_back(coords);

        if(marker!=0){
            auto ret = mesh.MarkersCell0D.insert({marker, {id}});
            if(!ret.second)
                (ret.first)->second.push_back(id);
        }
    }
    return true;
}

bool ImportCell1D(const string &filename, PolygonalMesh& mesh){
    ifstream file;
    file.open(filename);
    if(file.fail())
        return false;

    list<string> lines;
    string line;
    while(getline(file,line)){
        replace(line.begin(), line.end(), ';', ' ');
        lines.push_back(line);
    }
    lines.pop_front();
    file.close();

    mesh.NumberCell1D = lines.size();
    if(mesh.NumberCell1D == 0){
        cerr <<"There is no cell 1D"<<endl;
        return false;
    }
    mesh.IdCell1D.reserve(mesh.NumberCell1D);
    mesh.VerticesCell1D.reserve(mesh.NumberCell1D);

    for(const string& line:lines){
        istringstream convert(line);
        unsigned int id;
        unsigned int marker;
        Vector2d vertices;
        convert >> id >> marker >> vertices(0) >> vertices(1);
        mesh.IdCell1D.push_back(id);
        mesh.VerticesCell1D.push_back(vertices);

        if(marker!=0){
        auto ret = mesh.MarkersCell1D.insert({marker, {id}});
        if(!ret.second)
            (ret.first)->second.push_back(id);
        }

    }
    return true;
}

bool ImportCell2D(const string &filename, PolygonalMesh& mesh){
    ifstream file;
    file.open(filename);
    if(file.fail())
        return false;

    list<string> lines;
    string line;
    while(getline(file,line)){
        replace(line.begin(), line.end(), ';', ' ');
        lines.push_back(line);
    }
    lines.pop_front();
    file.close();

    mesh.NumberCell2D = lines.size();
    if(mesh.NumberCell2D == 0){
        cerr <<"There is no cell 2D"<<endl;
        return false;
    }
    mesh.IdCell2D.reserve(mesh.NumberCell2D);
    mesh.VerticesCell2D.reserve(mesh.NumberCell2D);
    mesh.EdgesCell2D.reserve(mesh.NumberCell2D);

    for(const string& line:lines){
        istringstream convert(line);
        unsigned int id;
        unsigned int marker;
        unsigned int dim;
        convert >> id >> marker >> dim;
        vector<unsigned int> vert(dim);
        vector<unsigned int> edg(dim);

        for(unsigned int i = 0; i<dim ; i++)
            convert >> vert[i];
        convert >> dim;
        for(unsigned int i = 0; i<dim ; i++)
            convert >> edg[i];

        mesh.IdCell2D.push_back(id);
        mesh.VerticesCell2D.push_back(vert);
        mesh.EdgesCell2D.push_back(edg);

        if(marker!=0){
            auto ret = mesh.MarkersCell2D.insert({marker, {id}});
            if(!ret.second)
                (ret.first)->second.push_back(id);
        }

    }
    return true;
}

bool ImportMesh(const string& filepath, PolygonalMesh& mesh){
    if(!ImportCell0D(filepath + "/Cell0Ds.csv", mesh))
        return false;
    else{
        cout << "Cell0D marker:" << endl;
        for(auto it = mesh.MarkersCell0D.begin(); it!=mesh.MarkersCell0D.end(); it++){
            cout << "key:\t" << it -> first << "\t values: ";
            for(const unsigned int id : it->second)
                cout << "\t" << id;
            cout << endl;
        }
    }

    if(!ImportCell1D(filepath + "/Cell1Ds.csv", mesh))
        return false;
    else {
        cout << "Cell1D marker:" << endl;
        for(auto it = mesh.MarkersCell1D.begin(); it!=mesh.MarkersCell1D.end(); it++){
            cout << "key:\t" << it -> first << "\t values: ";
            for(const unsigned int id : it->second)
                cout << "\t" << id;
            cout << endl;
        }
    }

    if(!ImportCell2D(filepath + "/Cell2Ds.csv", mesh))
        return false;
    else{

        for(unsigned int i=0; i<mesh.NumberCell2D; i++){
            vector<unsigned int> edg = mesh.EdgesCell2D[i];

            //cout << mesh.EdgesCell2D[i][0];
            for(unsigned int j=0; j<edg.size(); j++){

                const unsigned int origin = mesh.VerticesCell1D[edg[j]][0];
                const unsigned int end = mesh.VerticesCell1D[edg[j]][1];

                //controllo che i vertici trovati siano affettivamente dei vertici del poligono
                auto findOrigin = find(mesh.VerticesCell2D[i].begin(), mesh.VerticesCell2D[i].end(), origin);
                auto findEnd = find(mesh.VerticesCell2D[i].begin(), mesh.VerticesCell2D[i].end(), end);

                if(findOrigin == mesh.VerticesCell2D[i].end()){
                    cerr << "Wrong mesh: the begin of the edge does not correspond to a vertex of the polygon" << endl;
                    return 2;
                }
                if(findEnd == mesh.VerticesCell2D[i].end()){
                    cerr << "Wrong mesh: the end of the edge does not correspond to a vertex of the polygon" << endl;
                    return 3;
                }

                //controllo che i lati abbiano lunghezza maggiore di zero
                if(origin == end){
                    cerr << "Wrong mesh: there is one edge of length zero" << endl;
                    return 4;
                }

                //controllo che abbia area diversa da zero
                //avendo già controllato che i singoli lati non siano uguali a zero devo solo controllare che siano almeno tre e che siano diversi tra loro
                if(edg.size()<3){
                    cerr << "Wrong mesh: the polygon has less than 3 edges";
                    return 5;
                }

                unsigned int count = 0;
                for(unsigned int h=0; h<edg.size(); h++){
                    if(edg[h]==edg[j])
                        count++;
                }
                if(count>1){
                    cerr << "Wrong mesh: the same edge is listes more than once";
                    return 6;
                }

            }

            //controlle che le aree siano tutte diverse da zero
            vector<unsigned int> vert = mesh.VerticesCell2D[i];
            vector<Vector2d> coorVert = {};
            coorVert.reserve(vert.size());
            double sum = 0.0;
            for(unsigned int h=0; h<vert.size(); h++){
                coorVert.push_back(mesh.CoordinatesCell0D[vert[h]]);
            }
            coorVert.push_back(coorVert[0]);
            for(unsigned int h=0; h<vert.size()+1; h++){
                sum+=(coorVert[h][0]*coorVert[h+1][1]) - (coorVert[h+1][0]*coorVert[h][1]);
            }
            double area = abs(sum)/2;
            //cout << "area: " << area<< endl;

            if(area<=0){
                cerr << "Wrong mesh: one of the polygon has null area";
                return 7;
            }
        }
        /*cout << "Cell2D marker:" << endl;
        for(auto it = mesh.MarkersCell2D.begin(); it!=mesh.MarkersCell2D.end(); it++){
            cout << "key:\t" << it -> first << "\t values: ";
            for(const unsigned int id : it->second)
                cout << "\t" << id;
            cout << endl;
        }*/
    }
    return true;
}
}
