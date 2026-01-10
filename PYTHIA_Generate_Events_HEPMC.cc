#include <iostream>
#include <string>
#include <unordered_map>
#include "Pythia8/Pythia.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/FourVector.h"
#include "HepMC3/WriterAscii.h"

std::unordered_map<std::string, std::string> particle_mapping_dictionary
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

int main() {
    std::string idA_input, idB_input, subfolder;

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
    
    int nevents = 1e4;

    Pythia8::Pythia pythia;
    
    pythia.readString("Beams:idA = " + std::to_string(idA));
    pythia.readString("Beams:idB = " + std::to_string(idB));
    pythia.readString("Beams:eCM = 14.e3");
    pythia.readString("SoftQCD:all = on");
    pythia.readString("HardQCD::all = on");

    pythia.init();

    if (std::to_string(idA).length() == 4 && std::to_string(idB).length() == 4) {
        subfolder = "Moller_like";
    } else if (std::to_string(idA).length() == 3 || std::to_string(idB).length() == 3) {
        subfolder = "Creation_and_Annihilation";
    }
    
    HepMC3::WriterAscii writer(("HEPMC_Results/" + subfolder + "/Simulation_" + idA_input + "_and_" + idB_input + ".hepmc").c_str());

    for (int i = 0; i < nevents; i++) 
    {
        if (!pythia.next()) continue;

        HepMC3::GenEvent event;

        // Convert Pythia event to HepMC format
        for (int j = 0; j < pythia.event.size(); j++) {
            Pythia8::Particle& p = pythia.event[j];
            
            // Create HepMC particle
            HepMC3::GenParticlePtr hep_particle = std::make_shared<HepMC3::GenParticle>(
                HepMC3::FourVector(p.px(), p.py(), p.pz(), p.e()),
                p.id(),
                p.status()
            );
            
            // Add the particle to the HepMC event
            event.add_particle(hep_particle);
        }

        // Write the event to the HepMC file
        writer.write_event(event);

        std::cout << "Event " << i << " written to HEPMC file" << std::endl;
    }

    std::cout << "All events saved" << std::endl;
    return 0;
}