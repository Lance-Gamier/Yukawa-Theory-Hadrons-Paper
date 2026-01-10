#include <iostream>
#include <string>
#include <unordered_map>
#include "TFile.h"
#include "TTree.h"
#include "Pythia8/Pythia.h"

std::unordered_map <std::string, std::string> particle_mapping_dictionary
{
    // Baryon
    {"proton","2212"}, // (uud)
    {"sigma_plus", "3222"}, // (uus)
    {"sigma_plus_plus_c", "4222"}, // (uuc)
    {"sigma_asterisk_plus_b", "5224"}, // (uub)
    {"neutron", "2112"}, // (ddu)
    {"sigma_minus", "3112"}, // (dds)
    {"xi_zero_c", "4132"}, // (ddc)
    {"sigma_minus_b", "5112"}, // (ddb)
    {"xi_zero", "3322"}, // (ssu)
    {"xi_minus", "3312"}, // (ssd)
    {"omega_0_c", "4332"}, // (ssc)
    {"omega_minus_b", "5332"}, // (ssb)
    {"xi_plus_plus_c_c", "4422"}, // (ccu)
    {"xi_plus_c_c", "4412"}, // (ccd)
    {"omega_plus_c_c", "4432"}, // (ccs)
    {"omega_plus_b_c_c", "5442"}, // (ccb)
    {"xi_0_b_b", "5522"}, // (bbu)
    {"xi_-_b_b", "5512"}, // (bbd)
    {"omega_minus_b_b", "5532"}, // (bbs)
    {"omega_0_b_b_c", "5542"}, // (bbc)
    // Meson
    {"pion_0", "111"}, // (u -d)
    {"pion_+", "211"}, // (d -u)
    {"D_0", "421"}, // (c -u)
    {"D_+", "422"}, // (c -d)
    {"K_0", "311"}, // (d -s)
    {"K_+", "321"}, // (u -s)
    {"D_+_s", "431"}, // (c -s)
    {"B_0", "511"}, // (d -b)
    {"B_+", "521"}, // (u -b)
    {"B_0_s", "531"}, // (s -b)
    {"B_+_c", "541"} // (c -b)
};

int main()
{
    std::string idA_input, idB_input;

    std::cout << "Baryons:\n";
    for (const auto& [name, code] : particle_mapping_dictionary)
    {
        if (code.length() == 4)
        {
            std::cout << "  " << name << '\n';
        }
    }

    std::cout << "\nMesons:\n";
    for (const auto& [name, code] : particle_mapping_dictionary)
    {
        if (code.length() == 3)
        {
            std::cout << "  " << name << '\n';
        }
    }

    std::cout << "Enter the name of the first particle: ";
    std::cin >> idA_input;
    
    std::cout << "Enter the name of the second particle: ";
    std::cin >> idB_input;

    auto idA_str = particle_mapping_dictionary.find(idA_input);
    auto idB_str = particle_mapping_dictionary.find(idB_input);
    
    int idA = std::stoi(idA_str->second);
    int idB = std::stoi(idB_str->second);
    
    // Open ROOT file to store the tree
    TFile *output = new TFile(("ROOT_Results/Simulation_" + idA_input + "_and_" + idB_input + ".root").c_str(), "recreate");

    // Create a TTree to hold the data
    TTree *tree = new TTree("tree", "tree");

    // Declare variables for branches
    int id, event, size, no;
    double m, px, py, pz;

    // Create branches in the tree
    tree->Branch("event", &event, "event/I");
    tree->Branch("size", &size, "size/I");
    tree->Branch("no", &no, "no/I");
    tree->Branch("id", &id, "id/I");
    tree->Branch("m", &m, "m/D");
    tree->Branch("px", &px, "px/D");
    tree->Branch("py", &py, "py/D");
    tree->Branch("pz", &pz, "pz/D");

    int nevents = 1e4;

    // Initialize Pythia
    Pythia8::Pythia pythia;

    pythia.readString("Beams:idA = " + std::to_string(idA));
    pythia.readString("Beams:idB = " + std::to_string(idB));
    pythia.readString("Beams:eCM = 14.e3");
    pythia.readString("SoftQCD:all = on");
    pythia.readString("HardQCD::all = on");

    Pythia8::Hist hpz("Momentum Distribution", 100, -10, 10);

    pythia.init();

    // Loop over events
    for(int i = 0; i < nevents; i++)
    {
        if(!pythia.next()) continue;  // Skip the event if it's not successful
        int entries = pythia.event.size();  // Get the number of particles in the event
        
        event = i;
        size = entries;

        // Loop over particles in the event
        for(int j = 0; j < entries; j++)
        {
            id = pythia.event[j].id();  // Fix: Get the particle ID, not mass

            no = j;   // Particle index

            m = pythia.event[j].m();   // Particle mass
            px = pythia.event[j].px();  // Particle px
            py = pythia.event[j].py();  // Particle py
            pz = pythia.event[j].pz();  // Particle pz

            hpz.fill(pz);  // Fill histogram with pz value
        }

        tree->Fill();  // Fill the tree with the current event's data
    }

    // Write the tree to the ROOT file
    output->Write();
    output->Close();

    return 0;
}
