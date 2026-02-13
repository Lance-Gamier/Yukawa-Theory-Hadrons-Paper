void read_histogram(const std::string& filename)
{
    TFile *file = TFile::Open(filename.c_str());

    if (!file || file->IsZombie())
    {
        std::cerr << "Failed to open ROOT file" << std::endl;
        return;
    }

    TTree *tree = (TTree*)file->Get("tree");

    if (!tree)
    {
        std::cerr << "Tree not found!" << std::endl;
        return;
    }

    TH1F *hist;
    float no;
    tree->SetBranchAddress("no", &hist);

    Long64_t maxEntries = 10000;
    Long64_t nEntries = std::min(tree->GetEntries(), maxEntries);
    for (Long64_t i = 0; i < nEntries; ++i)
    {
        tree->GetEntry(i);
        hist->Fill(no);
    }

    if(hist)
    {
        hist->Draw();
    }
    else
    {
        std::cerr << "Histogram not found in the tree!" << std::endl;
    }

    hist->Draw();

    file->Close();
}