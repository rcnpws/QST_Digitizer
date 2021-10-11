{
  
  TChain *ch2 = new TChain("tree2");
  TChain *ch3 = new TChain("tree3");

  /*
  ch2->Add("./root/run1101.root");
  ch2->Add("./root/run1102.root");
  ch2->Add("./root/run1103.root");
  ch2->Add("./root/run1104.root");
  ch2->Add("./root/run1105.root");
  ch2->Add("./root/run1106.root");
  ch2->Add("./root/run1107.root");

  ch3->Add("./root/run1101.root");
  ch3->Add("./root/run1102.root");
  ch3->Add("./root/run1103.root");
  ch3->Add("./root/run1104.root");
  ch3->Add("./root/run1105.root");
  ch3->Add("./root/run1106.root");
  ch3->Add("./root/run1107.root");
  */

  //91Zr
  /*
  ch2->Add("./root/run1113.root");
  ch2->Add("./root/run1114.root");
  ch2->Add("./root/run1115.root");
  ch2->Add("./root/run1116.root");
  ch2->Add("./root/run1117.root");
  ch2->Add("./root/run1118.root");
  ch2->Add("./root/run1119.root");
  ch2->Add("./root/run1120.root");
  ch2->Add("./root/run1121.root");
  ch2->Add("./root/run1122.root");
  ch2->Add("./root/run1123.root");  
  */
  
  //94Zr
  /*
  ch3->Add("./root/run1133.root");
  ch3->Add("./root/run1134.root");
  ch3->Add("./root/run1135.root");
  ch3->Add("./root/run1136.root");
  ch3->Add("./root/run1137.root");
  ch3->Add("./root/run1138.root");
  ch3->Add("./root/run1139.root");
  ch3->Add("./root/run1140.root");
  ch3->Add("./root/run1141.root");
  ch3->Add("./root/run1142.root");
  */

  ch2->Add("./root/run1143.root");
  ch2->Add("./root/run1144.root");
  
  ch2->Draw("GeE*1.0096>>(4000,0,2000)","","");
  // ch3->Draw("GeE*1.0114>>(4000,0,2000)","","");

}
