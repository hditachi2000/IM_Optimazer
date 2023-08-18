#include<iostream>
#include<vector>
#include<unordered_set>
#include<fstream>
#include<sstream>
#include<ctime>
#include<cstdlib>
#include<string>
#include<random>
#include<algorithm>
#include<unordered_map>
#include<queue>
#include<chrono>
#include<thread>

using namespace std;

#define numfiles 5
#define interfer 0.7        // giao thoa follower 
#define amp 0.5             // biên độ price
//#define ratioH 0.8           // tỉ trọng Heristic
#define partial  0.3        // thiên vị số lượng follower / khoảng dao dộng cho phép đối với standard follower
#define seedFollower 2000    
#define learning_rate 0.005;     

struct Input{
    int numFamous;    
    int fund;  
    vector<int> ids;
    vector<int> price;
    vector<vector<int>> followers;

    Input(){};
    Input(string filename){
        Docfile(filename);
        //printData(filename);
    }

    void Docfile(string filename){
        clearInput();                     // xóa dữ liệu trước khi đọc file mới 
        ifstream file(filename);
        if (file.is_open()) {
        // Đọc dữ liệu từ file
            file >> fund;
            file >> numFamous;
            string line;
            istringstream ss(line);
            getline(file,line);
            while(getline(file,line)){
                istringstream iss(line);                // đọc tới cuối file
                int id;
                float money;
                iss >> id;
                iss >> money;
                vector<int> follow;
                ids.push_back(id);
                price.push_back(money);
                int f;
                while (iss >> f) {
                    follow.push_back(f);
                }
                followers.push_back(follow);
            }
            file.close();                   
        }
        else cout << "Unable to open file";
    }
    void clearInput(){
        this->numFamous = 0;
        this->ids.clear();
        this->price.clear();
        for(auto& follow : this->followers){
            follow.clear();
        }
        this->followers.clear();
    }
    void Display(){
        cout << "NumFamous: " << numFamous << endl;
        cout << "Total money: " << fund << endl;
        for (auto i : ids) {
            cout << "ID: " << i << ", price: " << price[i-1] << ", Followers: ";
            for (int j = 0; j < followers[i].size(); j++) {
                cout << followers[i].size() << ", Followers: ";
                cout << followers[i][j] << " ";
            }
            cout << endl;
        }
    }


};

// NOTE : rand() của ide c++ visual ko thể tạo số ngẫu nhiên quá lớn ( cỡ >100000) , tb_money thuê 1 Famous buộc phải bị giảm xuống 
void Tao1File(string filename){ 
    int numFamous = rand()%101+100;                     // khởi tạo số người nổi tiếng ngẫu nhiên từ 50-100
    long fund = rand()%81 + 20;              // Khởi tạo tổng số tiền được cấp từ 20.000.000 - 100.000.000
    fund = fund*1000000;                      // làm tròn tới hàng nghìn
    vector<int> ids;
    vector<int> price;
    vector<vector<int>> followers;        
    //float interfer = 0.5 ;                     // giao thoa : có khoảng 50% follower theo dõi hơn 2 người nổi tiếng 
                                                // hoặc 2 Famous sẽ trùng nhau khoảng 50% follower
    float tb = fund/numFamous;                 // trung bình số tiền được cấp trên mỗi người nổi tiếng
    int MaxNumFollowers = numFamous*seedFollower*(1+partial/2);   // Tổng số followers trong data (dự đoán)
    //float amp = 0.5;    // tỉ lệ dao động cho phép giữa tiền thuê và tiền thuê trung bình trong lúc tạo ngẫu nhiên 
                         // (1+amp)/min(tile) phải nhỏ hơn 1 . ví dụ : 1.5/2 = 0.75 , tránh t/h tệ nhất ngẫu nhiên sinh ra thuê hết
                        // người nổi tiếng mà chưa hết tiền
    // Mỗi file data sẽ có 1 tỉ lệ riêng
    float tile = rand()%26+20;
    tile = tile/10;                                      //tỉ lệ tiền thuê mỗi người / số tiền trung bình (2-4.5)
                                                        //ví dụ : tỉ lệ là 2 : chỉ đủ tiền để chọn được khoảng 50% số người nổi tiếng 
    int tb_money = (int)tile * tb;                     // tiền thuê trung bình 1 người nổi tiếng
    // Tạo dữ liệu ngẫu nhiên cho mỗi người nổi tiếng
    for (int i = 0; i < numFamous; i++) {    
        int id = i + 1;                                                                                                                          
        int money = rand()%(tb_money/1000) + amp*(tb_money/1000);      // số tiền thuê dao động từ 0.5 . 1.5 số tiền trung bình (tb_money) / max ~ 3*min price 
        money = money*1000;                  // làm tròn
        price.push_back(money);
        ids.push_back(id); 
        vector<int> follow; 
        float balance =(float) money / tb_money;     //chỉ số tránh thiên vị sinh ra do ngẫu nhiên : giá thuê quá cao nhưng quá ít follower 
        //cout << "balance: " << balance << endl;     // balance dao động trong khoảng (amp ,1+amp)
        int StandNumFollower = balance * seedFollower;
        // khởi tạo số follower của 1 Famous từ 40 -> 144 ( với seed = 80)
        int numFollowers = StandNumFollower + rand()%(int)(StandNumFollower * partial) ; 
        // nếu balance = 0.5 ~ 40 , nếu balance = 1.5 ~ 120
        while(numFollowers>0) {                        // Chọn ngẫu nhiên người theo dõi
            int f = rand() % MaxNumFollowers*(1-interfer/2) + 1;               // chọn ngẫu nhiên follower với miền giao thoa
            if(find(follow.begin(),follow.end(),f) == follow.end()){    // ktra xem đã xuất hiện trong danh sach theo dõi chưa
                follow.push_back(f);  
                numFollowers--;
            }
        }
        followers.push_back(follow);
    }
    // Tạo file và ghi dữ liệu vào file
    ofstream file(filename);
    if (file.is_open()) {
        file << fund << endl;                   // dòng đầu tiên là tổng số tiền
        file << numFamous << endl;              // dòng 2 là số người nổi tiếng 
        for (int i = 0; i < numFamous; i++) {
            file << ids[i] << " " << price[i] << " ";
            for (int j = 0; j < followers[i].size(); j++) {
                file << followers[i][j] << " ";
            }
            file << endl;
        }
        cout << "Done make file " << filename <<" !" << endl; 
        file.close();
    }
    else{
    cout << "Cannot write into file !" << endl;
    }
}

void TaoNhieuFile(){
    for(int i=1; i<=numfiles; i++){
        string filename = "dataIM" + to_string(i) + ".txt";
        Tao1File(filename);
    }
}

void printData(string filename){
    int numFamous;                  
    int fund;                        
    vector<int> ids;
    vector<int> price;
    vector<vector<int>> followers;
    ifstream file(filename);
    if (file.is_open()) {
    // Đọc dữ liệu từ file
        file >> fund;
        file >> numFamous;
        string line;
        istringstream ss(line);
        getline(file,line);
        while(getline(file,line)){
            istringstream iss(line);             
            int id;
            float money;
            iss >> id;
            iss >> money;
            vector<int> follow;
            ids.push_back(id);
            price.push_back(money);
            int f;
            while (iss >> f) {            // đọc liên tiếp các follower của 1 ng nổi tiếng 
                follow.push_back(f);;
            }
            followers.push_back(follow);
        }
    // In ra dữ liệu
    int numFollower =0;
    for(auto f : followers){
        numFollower += f.size();
    }
    cout << "NumFamous: " << numFamous << endl;
    cout << "Total money: " << fund << endl;
    cout << "Total Follower: " << numFollower << endl;
    //for (int i = 0; i < numFamous; i++) {
        //cout << "ID: " << ids[i] << ", price: " << price[i] << ", followers ";
        //cout << "(" << followers[i].size() <<") :" ;
        //for (int j = 0; j < followers[i].size(); j++) {
            ////cout << followers[i][j] << " ";
        //}
        //cout << endl;
    //}
    //file.close();
    //}
    //else {
        //cout << "Unable to open file" << endl;
    }
}

struct Solution{
    Input input; 
    unordered_set<int> selected;     // những người nổi tiếng đã được chọn
    unordered_set<int> left;        // những người nổi tiếng chưa được chọn 
    int Used ;                    // tiền đã tiêu
    int score ;                  // số người đã được chọn 
    unordered_map<int,int> store;                 // danh sách follower đã chọn và số lần được chọn 
    float ratioH = 0.0;                 // tỉ lệ kết hợp heristic 
    vector<float> h1;
    vector<float> h2;
    unordered_map<int, unordered_set<int>> data;             // sao chép dữ liệu, phục vụ cho tăng tốc khởi động

    void setLeft(){                           // thêm tất cả Famous vào danh sách chưa được chọn 
        for(auto x : input.ids){
            left.insert(x);
        }

    }
    void clearSolution(){     // setup lại lời giải về ban đầu
        this->Used = 0;
        this->score = 0;
        this->left.clear();
        this->selected.clear();
        this->store.clear();
        setLeft();
    } 
    Solution(string filename){
        cout << "Loading Solution ..." << endl;
        this->input = Input(filename);  
        makeData();
        clearSolution();
        VecHeristic1();     // thiết lập vector heristic1 
        VecHeristic2();     // thiết lập vector heristic2
        data.clear();         // giải phóng bộ nhớ 
        cout << " Done make Solution !" << endl;
    }
    void firstSolution(){  
        int pickID;
        clearSolution();                              // tạo lời giải ban đầu , id ngẫu nhiên
        while(Used <= input.fund){                     // chọn tới khi hết quỹ 
            pickID = rand()%input.numFamous +1;   // chọn id ngẫu nhiên 
            if(selected.find(pickID) == selected.end()) continue;         // bỏ qua những ID đã được chọn
            AddFamous(pickID);
        }
        if(Used > input.fund) pushFamous(pickID);
    }
  
    void AddFamous(int id){                                     // thêm Famous vào danh sách chọn
        selected.insert(id);
        left.erase(id);
        Used += input.price[id-1];
        for(auto x : input.followers[id-1]){
            store[x]++;                            // thêm x phần tử vào store, tăng giá trị xuất hiện lên 1
        }
        score = store.size();
    }

    void pushFamous(int id){                              // bỏ chọn Famous 
        if(selected.count(id)>0){         // ktra xem có trong danh sách chọn chưa 
            selected.erase(id);
            left.insert(id);
            Used -= input.price[id-1];
            for(auto x : input.followers[id-1]){
                store[x]--;
                if(store[x] == 0) store.erase(x);
            }
        }
        score = store.size();
    }
    
    // tìm id của famous có phỏng đoán h1 tối ưu
    int GoodHeristic(unordered_set<int> u){
        float minRatio = 9999; 
        int minID = 0;
        for(auto id : u){
            float ratio  = input.price[id-1] / input.followers[id-1].size();   
            if(ratio < minRatio){
                minRatio = ratio;
                minID = id;
            }
        }                 
        return minID;
    }

    // tạo lời giải ban đầu với heristic 1 : số follower / giá thuê lớn được coi là tốt
    void firstHeristic(){     
        clearSolution();            
        unordered_set<int> newIds;                  // tạo danh sách id quy về heristic
        for(auto x:input.ids){
            newIds.insert(x);
        }
        int id;
        while(Used <= input.fund){       // bắt đầu chọn Famous với heristic giảm dần
            id = GoodHeristic(newIds);
            AddFamous(id);                  // thêm id famous vào danh sách chọn
            newIds.erase(id);              // xóa id vừa chọn khỏi danh sách id heristic
        }
        if(Used > input.fund) pushFamous(id);    
    }

    // NOTE : Heristic 2 : một follower theo dõi nhiều Famous. Famous chứa nhiều follower đó là tệ 

    // hàm trả về số idol của 1 follower
    // hàm trả về số idol của 1 follower
    int findRepeat(int idf){
        int count = 0;
        for(int id =1; id <= input.numFamous ;id++){
            if(data[id].find(idf) != data[id].end()){
                count++;
            }
        }
        return count;
    }

    /* NOTE : Heristic 3 : Tính độ tệ của 1 Famous dựa trên tổng độ tệ của 1 follower
                           Độ tệ của 1 follower là 1 tỉ lệ (0-1) : số idol của người đó / tổng số idol( NumFamous)
                           Tỉ lệ này đại diện cho khả năng bị trùng lặp follower của 1 Famous với "tất cả" những Famous còn lại
    */

    // hàm tạo 1 bản sao data của input : Việc khởi tạo vector H2 có độ phức tạp N^2 rất tốn thời gian với vector . 1 bản sao sử dụng map và set
    // sẽ giảm thời gian xuống còn O(N) . sau khi thiết lập xong h1,h2 tại solution(), data sẽ được xóa bỏ để tiết kiệm bộ nhớ 
    void makeData(){
        int countID = 1;
        for(auto follow : input.followers){
            data[countID].insert(follow.begin(),follow.end());
            countID++;
        }
    }
    // Hàm trả về %: số idol của 1 follower / tổng số idol(numFamous) 
    float findPercent(int idf){
        return (float)findRepeat(idf)/input.numFamous;
    }

    // note1: 1 follower có trên 1 idol , khi tính tổng độ tệ của Famous, liệu có tính tổng số trùng lặp của từng follower , hay 
    // mỗi follower trùng lặp chỉ tính + 1 điểm tệ bất kể số idol theo dõi của follower đó

    // heristic 2.1 : tổng (số idol của 1 follower) của tất cả follower / tổng số follower . Tính trên 1 Famous
    // Heristic 2.2 : Tổng số follower bị trùng(có > 2 idol) / tổng số follwer

    // hàm trả về độ tệ của 1 vector - Heristic 2
    float repeatVector(vector<int> u){
        float num = u.size();
        float numRepeat = 0;
        for(auto f : u){
            numRepeat += findPercent(f);            
        }
        return numRepeat/num;
    }

    // hàm thiết lập vector chứa % về độ tốt đối với heristic1 của tất cả Famous  (từ 0 -> 1)
    void VecHeristic1(){
        int max = -999999;
        int min = 999999;           
        for(auto id : input.ids){
            float ratio  = input.price[id-1] / input.followers[id-1].size();  
            if(ratio > max) max = ratio; 
            if(ratio < min)  min = ratio; 
            h1.push_back(ratio);                           
        }
        // chuẩn hóa 0->1 
        for(auto& h : h1){
            h -= min;
            h /= (max-min);            // tỉ lệ càng gần 0 càng tốt, càng gần 1 càng tệ 

        }
    }
    // NOTE : h1 và h2 phải đồng biến, nghĩa là càng gần 0 hoặc 1 thì càng tốt (tệ)

    // hàm thiết lập vector chứa % về độ tệ đối với heristic 2 của tất cả Famous ( từ 0 . 1)
    void VecHeristic2(){
        float max = -1, min =1;
        for(auto id : input.ids){
            float r = repeatVector(input.followers[id-1]);
            //r = 1/r;                        // nghịch biến h1, vì r càng lớn càng tệ. Phải đồng biến với h1
            if(r>max) max = r;
            if(r<min) min = r;
            h2.push_back(r);
        }
        // chuẩn hóa về dạng 0->1
        for(auto& h : h2){
            h -= min;
            h /= (max-min);
        }
    }

    // Hàm kết hợp 2 heristic , trả về 1 vector chứa heristic tổng hợp của các famous 
    vector<float> VecHeristic(){
        vector<float> H ;
        for(int i=0; i<input.numFamous;i++){
            float kq = ratioH*h1[i] + (1-ratioH)*h2[i];
            H.push_back(kq);
        }
        return H;           // trọng số H càng lớn là càng tệ
    }

    // hàm con : tìm ID có heristic tổng hợp tốt nhất
    int getMinIndex(unordered_map<int,float> a){
        float min = 999;
        int id = 0;
        for(auto x : a){
            if(x.second <min){
                min = x.second;
                id = x.first;
            }
        }
        return id;
    }
    
    // hàm con: tìm ID có heristic tổng hợp tệ nhất
    int getMaxIndex(unordered_map<int,float> a){
        float max = -999;
        int id = 0;
        for(auto x : a){
            if(x.second > max){
                max = x.second;
                id = x.first;
            }
        }
        return id;
    }
    // tạo lời giải ban đầu với heristic tổng hợp
    void MasterFirstSolution(){
        clearSolution();
        vector<float> H0 = VecHeristic();  
        unordered_map<int,float> H;
        for(int id = 0; id < H0.size() ;id++){
            H.insert(make_pair(id,H0[id]));
        }

        int minID;
        do{
            minID = getMinIndex(H);   // tìm id của Famous có heristic tổng hợp tốt nhất 
            AddFamous(minID+1);
            H.erase(minID);
        }while(Used <= input.fund);
        if(Used > input.fund) pushFamous(minID+1); 
    }
    
    // Hàm chính : tìm tỉ lệ kết hợp tốt nhất giữa 2 heristic 
    void training(){
        cout << "Preparing to training.... " << endl;
        MasterFirstSolution();
        int bestScore = this->score;
        float bestRatioH = 0;
        bool direction = true;
        int count = 0;
        int count1 = 0;
        while(true){
            count1++;
            //cout << "Epoch "<< count1 << " - ";
            int oldScore = this->score;
            //ratioH += 0.005;
            if(direction){
                ratioH += learning_rate;
            }
            else ratioH -= learning_rate;
            cout << "Ratio: " << ratioH << "-";
            MasterFirstSolution();
            int newScore = this->score;
            cout << " New score: " << newScore << endl;
            if(newScore >= bestScore){
                if(newScore > bestScore){
                    bestScore = newScore;
                    bestRatioH = ratioH;
                    cout << " New increase Score: " << bestScore << endl;
                    count = 0;
                }
            }
            // đổi hướng 
            else {   
                count ++;
                //direction = !direction;
            }
            // quay đầu
            if(ratioH < 0 || ratioH > 1) direction = !direction;
                //break;
            // dừng khi không cải thiẹn , chạy quá lâu
            if(count1 >= 200) break;
        }
        cout << "Best Score :" << bestScore << endl;
        cout << "best ratioH: " << bestRatioH << endl;
        this->ratioH = bestRatioH;                           // lưu lại tỉ lệ tối ưu cho các lần training sau
        MasterFirstSolution();         // thực hiện lại Solution khi đã tìm được tỉ lệ tối ưu
    }   
   
    // hàm con phụ trợ, sử dụng trong Swap_1n1 và reborn : trả về 1 unordered_map chứa <id,heristic> của selected / left 
    unordered_map<int, float> makeUnorderHeristic(unordered_set<int> a){
        unordered_map<int, float> Repeat ;      //
        vector<float> H = VecHeristic();
        for(auto id : a){
            float her = H[id-1];
            Repeat[id]= her;
        }
        return Repeat;
    }

    // Hàm con, phụ trợ cho LocalSearch() : Tạo 1 queue chứa tập Heristic của tập selected 
    queue<int> makeQueueSelect(){
        unordered_map<int, float> Her2Select = makeUnorderHeristic(selected);
        queue<int> queSelect;
        int minIDSelect;
        float max = -1;
        while(!Her2Select.empty()){
            for(auto f1 : Her2Select){
                if(f1.second >= max){
                    max = f1.second;
                    minIDSelect = f1.first;
                }
            }
            max = -1;
            queSelect.push(minIDSelect);
            Her2Select.erase(minIDSelect);
           
        }
        return queSelect;
    }
    
    // Hàm con, phụ trợ cho LocalSearch() : Tạo 1 queue chứa tập Heristic của tập left
    queue<int> makeQueueLeft(){
        unordered_map<int,float> Her2Left = makeUnorderHeristic(left);
        queue<int> queLeft;
        int maxIDLeft;
        float min = 2;
        while(!Her2Left.empty()){
            for(auto f1 : Her2Left){
                if(f1.second <= min){
                    min = f1.second;
                    maxIDLeft = f1.first;
                }
            }
            min = 2;
            queLeft.push(maxIDLeft);
            Her2Left.erase(maxIDLeft);
           
        }
        return queLeft;
    }
    
    // Hàm swap 1:n:1
    void Swap_1n1(int realback,unordered_map<int,unordered_set<int>>& theBest){
        int back = realback;
        int bestScore = this->score;
        queue<int> queSelect = makeQueueSelect();
        queue<int> queLeft = makeQueueLeft();
        queue<int> CoppyLeft = queLeft;
        unordered_map<int,unordered_set<int>> bestSwap;

        do{
            int id1 = queSelect.front();
            unordered_set<int> added;       // tập set lưu lại những id đã được thêm vào
            do{
                pushFamous(id1);
                while(Used <= input.fund){            // lấy Famous từ LEft cho tới khi đầy quỹ
                    if(queLeft.size() < 1) break;
                    int id2 = queLeft.front();
                    if(Used + input.price[id2-1]> input.fund){
                        break;  //dừng khi tràn quỹ
                    }else{
                        AddFamous(id2);
                        added.insert(id2);
                        queLeft.pop();
                    }
                }
                int newScore = this->score;
                if(newScore > bestScore-realback){
                    realback = 0;                       // chỉ lùi 1 lần ban đầu 
                    bestSwap.clear();
                    bestScore = newScore;
                    bestSwap[id1] = added;
                }
                AddFamous(id1);
                for(auto id : added){
                    pushFamous(id); 
                }
                added.clear();
                if(queLeft.size() > 0) queLeft.pop();

            }while(!queLeft.empty());
            queSelect.pop();
            queLeft = CoppyLeft;            // khôi phục lại queLeft sau khi dùng hết
        }while(!queSelect.empty());
        // thực hiện swap cho lần swap tốt nhất đã được lưu
        if(bestSwap.size() > 0){
            for(auto id : bestSwap){
                pushFamous(id.first);
                for(auto idf : id.second){
                    AddFamous(idf);
                }
            }
        }  
        //cout << "______________________TURNNN______________________" << endl; 
        bestSwap.clear();       
        queue <int> empty;
        queLeft.swap(empty);
        queSelect.swap(empty);
        queLeft = makeQueueLeft();
        queSelect= makeQueueSelect();
        realback = back;                            // khôi phục lại realback
        bestScore = this->score;
        queue<int> CoppySelect = queSelect;
        do{
            int id1 = queLeft.front();
            unordered_set<int> added;       // tập set lưu lại những id đã được thêm vào
            do{
                AddFamous(id1);
                while(Used > input.fund){            
                    if(queSelect.size() < 1){        // trường hợp queSelect còn 1 ptu, lệnh break sẽ dừng vòng while ngay cả khi Used chưa về
                        break;                  // dưới mức fund . Điều này sẽ gây ra bug ngộ nhận cải thiện khi Used > fund vẫn dc chấp nhận
                    }
                    int id2 = queSelect.front();       
                    pushFamous(id2);
                    added.insert(id2);
                    queSelect.pop();
                }
                if(Used > input.fund){    //  khắc phục bug do lệnh break gây ra 
                    pushFamous(id1);
                    for(auto id : added){
                        AddFamous(id); 
                    }
                    added.clear();
                    if(queSelect.size() > 0) queSelect.pop();
                    continue;
                }
                int newScore = this->score;
                if(newScore > bestScore - realback){
                    realback =0;                                        // chỉ lùi 1 lần đầu tiên 
                    //cout <<"New best Score : " << newScore << endl;
                    realback = 0;
                    bestSwap.clear();
                    bestScore = newScore;
                    bestSwap[id1] = added;
                }
                pushFamous(id1);
                for(auto id : added){
                    AddFamous(id); 
                }
                added.clear();
                if(queSelect.size() > 0) queSelect.pop();
            }while(!queSelect.empty());
            if(queLeft.size() > 0) queLeft.pop();
            queSelect = CoppySelect;            // khôi phục lại queSelect sau khi dùng hết
        }while(!queLeft.empty());
        
        // thực hiện swap cho lần swap tốt nhất đã được lưu
        if(bestSwap.size() > 0){
            for(auto id : bestSwap){
                AddFamous(id.first);
                for(auto idf : id.second){
                    pushFamous(idf);
                }
            }
        }
        cout << "Best score: " << bestScore << endl;
        cout << "USed: " << Used << endl;
        theBest[this->score] = this->selected;      // sao chép bộ solution tốt nhất        
        //Swap_1n1();
    }

    // Simulated annealing FAKE : Chấp nhận giảm nếu ko thể tăng - trượt xuống khỏi Local maximum . Kết quả tối ưu sẽ được lưu lại trước khi lùi 
    // Khi Hàm swap không thể cải thiện , chấp nhận swap cho cả những bộ swap khiến kết quả tệ đi 
    void LocalSearch(unordered_map<int,unordered_set<int>>& theBest){            // truyền vào bộ lưu trữ Solution của hàm cha/lastSolution 
        //cout << "Starting Local Search ...!" << endl;
        int bestScore = this->score;
        int preScore, lastScore;
        int presentScore = bestScore;
        int back = 0;           // giá trị chấp nhận đi lùi , mặc định ban đầu là 0 .
        int realback = 0;
        int count = 0;
        int count1 = 0;
        int globalScore = bestScore;
        unordered_map<int,unordered_set<int>> theSubBest;   // Bộ lưu trữ những Solution tốt nhất sau mỗi vòng lặp 
        while(true){
            cout << "New Loop of Localsearch ...." << endl;
            if(back > sqrt(bestScore/100)){
                break;
            }
            preScore = this->score;
            Swap_1n1(realback, theSubBest);
            lastScore = this->score;
            if(lastScore <= globalScore){
                cout <<" Start using simulated annealing !" << endl;
                count ++;
                back+= count;               // đi lùi với giá trị giảm dần 
                realback = sqrt(bestScore/100) - back;       // đi lùi : trượt xuống khỏi local maximum . giá trị lùi sẽ giảm nhanh dần 
            }else{
                globalScore = lastScore;
            }
            count++;
        }
        int max = -1;
        // chọn ra Solution tốt nhất trong bộ lưu trữ Solution 
        for(auto choice : theSubBest){         
            if(choice.first > max){
                max = choice.first;
            }
        }
        RestoreSolution(theSubBest[max]);
        theBest[max] = this->selected;            // Thêm bộ Solution tốt nhất vào bộ lưu trữ Solution của hàm cha 
        cout <<"Global Score of LocalSearch : " << this->score << endl;
    }

    //NOTE : Hàm LastSolution là level cao hơn của LocalSearch, khi simulated_annealing không đủ năng lượng (back/realback) để vượt qua quá nhiều
    //       nhiều local maximum ,việc Reborn sẽ được thực hiện . Tuy nhiên , việc tái sinh phải dựa trên cơ sở chắc chắn . Việc này được thực
    //       hiện bằng cách dựa trên tập tiên đoán(Heristic tổng hợp) có được sau quá trình simulated_annealing . Các tập tiên đoán sẽ được cập 
    //       nhật liên tục sau mỗi lần LocalSearch, do đó kết quả được cải thiện đáng kể vì chúng dẫm lên thành tựu của nhau .
    void LastSolution(){
        int count = 0;
        float reset = 0.15;    // tỉ lệ reBorn
        int bestScore;
        int globalScore;
        unordered_map<int,unordered_set<int>> theBest;        // Bộ lưu trữ Solution sau mỗi lần reBorn 
        bestScore =this->score;
        globalScore = bestScore;
        while(count<3){
            LocalSearch(theBest);
            if(this->score > globalScore){
                globalScore = this->score;
            }
            cout <<"===========REBORN================" << endl;
            reBorn(reset);
            cout << "Score after reborn: " << this->score << endl;; 
            //reset*=0.9;
            count++;
        }
        int max = -1;
        // chọn ra bộ Solution tốt nhất sau n lần reBorn
        for(auto choice : theBest){
            if(choice.first > max){
                max = choice.first;
            }
        }
        RestoreSolution(theBest[max]);        // Khôi phục  
        cout <<"Start Filling Solution ..." << endl;
        fillSolution();                    // điền đầy lần sau cuối 
    } 

  
    // Cập nhật :Khi thêm 1 Famous : xóa toàn bộ follower được thêm trong mỗi tập con của setRemaingFollower
    // return : remaining[id-1].size() : số follower sẽ được thêm mới của Famous(id);

    // ban đầu chưa có follower nào được chọn, tập remaining = tập input followers
    void setRemainingFollower(unordered_map<int,unordered_set<int>>& a){ 
        unordered_set<int> f;
        int count = 1;             // đồng bộ với id của Famous trong tập followers 
        for(auto follow : input.followers){  
            f.clear();
            for(auto id : follow){
                f.insert(id);
            }
            a[count] = f;
            count++;
        }
    }

    int getMaxRemainingID(unordered_map<int,unordered_set<int>> a){
        float max = -1;
        int maxID;
        for(auto id : a){
            float r = (float)id.second.size()/input.price[id.first-1];
            if(r > max){
                max = r;
                maxID = id.first;
            }
        }
        return maxID;
        // O(m) - numFamous  
    }
    // NOTE : Putting : chọn ra famous được xem là tốt nhất tại thời điểm xét chọn famous
    void puting(){
        cout << "Initializing Puting() Function .... " << endl;
        clearSolution();
        unordered_map<int,unordered_set<int>> RemainingFollower;
        setRemainingFollower(RemainingFollower);                        // N
        while(Used <= input.fund){
            if(RemainingFollower.size() < 1) break;
            int maxID = getMaxRemainingID(RemainingFollower);  // chọn ra id famous tốt nhất . O(m^2)
            AddFamous(maxID);                                // lấy famous
            if(Used > input.fund){
                pushFamous(maxID);
                RemainingFollower.erase(maxID);
                continue;
            }else{
            // cập nhật lại tập remaining 
                for(auto& famous : RemainingFollower){         // duyệt qua tất cả id famous trong tập remaining
                    for(auto id : input.followers[maxID-1]){ 
                        famous.second.erase(id);                   // duyệt qua và xóa tất cả follower đã được chọn nếu có thuộc famous
                    }
                }
                RemainingFollower.erase(maxID);
            }
        }
        cout <<"Done puting with Score: " << score << endl;
    }

    // Hàm khôi phục Solution với tập selected đầu vào 
    void RestoreSolution(unordered_set<int> select){
        clearSolution();
        for(auto id : select){
            AddFamous(id);
        }
    }
    // Hàm reset 1 phần Solution 
    void reBorn(float p){
        int numKill = p*selected.size();
        unordered_map<int,float> select = makeUnorderHeristic(selected);
        unordered_map<int,float> lef = makeUnorderHeristic(left);
        while(numKill > 0){
            int id = getMaxIndex(select);
            pushFamous(id);
            select.erase(id);
            numKill--;
        }
        while(true){
            int id = getMinIndex(lef);
            AddFamous(id);
            lef.erase(id);
            if(Used > input.fund){
                pushFamous(id);
                break;
            }
        }
    }
    void reBorn2(float p){
        random_device rd;
        mt19937 gen(rd());
        int numKill = p*selected.size();
        int numToSelect = numKill*2;
        unordered_map<int,float> select = makeUnorderHeristic(selected);
        unordered_map<int,float> lef = makeUnorderHeristic(left);
        vector<int> pick;
        while(numToSelect>0){
            int id = getMaxIndex(select);
            pick.push_back(id);
            select.erase(id);
            numToSelect--;
        }
        while(numKill > 0){
            uniform_int_distribution<> dist(0, pick.size() - 1);
            auto it = next(pick.begin(), dist(gen));
            pushFamous(*it);
            pick.erase(it);
            numKill--;
        }
        while(true){
            int id = getMinIndex(lef);
            AddFamous(id);
            lef.erase(id);
            if(Used > input.fund){
                pushFamous(id);
                break;
            }
        }
    }
    // NOTE : SIMULATED ANNEALING 
    // - local search sẽ ko cải thiện nếu vòng đệ quy trước đó ko có cải thiện
    // - đột phá : Reset 30%
    // - chấp nhận kết quả xấu : swap với bộ swap ko tốt nhất 
    // - thay đổi thành n-1-n : ko cải thiện cho lắm

    /* NOTE 2 :
    - Cơ chế Localsearch, simulated annealing(SA) và reborn trong chương trình này có thể áp dụng cho mọi bài toán tìm kiếm không gian trạng thái 
    - Khi năng lượng của SA cạn kiệt, có thể cấp lại năng lượng cho nó thay vì reborn lại, điều này giúp giảm đáng kể thời gian tìm kiếm 
    - Việc cấp lại năng lượng và tiêu hao năng lượng trong quá trình tìm kiếm theo 1 công thức chưa được tìm ra, cũng như tỉ lệ tái sinh và số lần
      tái sinh cần thiết 
    
   */
    // Hàm điền đầy kết quả : Sau khi có được kết quả tối ưu, nếu còn dư tiền, thử đổi famous để sử dụng hết tiền . KẾt quả cải thiện sẽ được lưu 
    void fillSolution(){
        int ids=-1, idl=-1;                             // cặp swap làm đầy và cải thiện kết quả  
        unordered_set<int> copSe = selected;
        unordered_set<int> copLe = left;
        int bestScore = this->score;
        int hole = input.fund - Used;
        for(auto id1 : copSe){
            pushFamous(id1);                    // nhấc ra 1 famous trong tập selected
            for(auto id2 : copLe){              // chọn 1 famous trong left
                if(input.price[id2-1] <= input.price[id1-1]) continue;      // bỏ qua những famous có price nhỏ hơn
                AddFamous(id2);
                int Use = this->Used;
                int newScore = this->score;
                pushFamous(id2);
                if(newScore > bestScore && Use <= input.fund){
                    cout << "New bestScore: " << newScore << endl;
                    bestScore = newScore;
                    ids = id1;
                    idl  =id2;
                }
            }
            AddFamous(id1);
        }
        if(idl!=-1 && ids!=-1){                 // tìm được cặp làm đầy 
            AddFamous(idl);
            pushFamous(ids);
            if(Used < input.fund){
                fillSolution();         // nếu chưa đầy . tiếp tục làm đầy
            }else{
                cout <<"Done fill solution !" << endl;
                return;
            }
        }
        else{                  // tại vòng hiện tại không tìm được cặp làm đầy, dừng lại . Đệ quy không còn tác dụng cho những vòng sau 
            cout <<"Done fill solution !" << endl;  
            return;    
        }
    }
    void printSolution(){
        cout << " Loading PrintSolution ...." << endl;
        cout << "So tien duoc cap\t: " << input.fund << endl;
        cout << "So tien thue la \t: " << Used << endl; 
        cout << "Tong so follower \t: " << score << endl;;
        //cout << " Danh sach nhung nguoi duoc thue: " << endl;
        //for(auto id : selected){
            //cout << "ID: " << id << "\t Price: " << input.price[id-1] ;
            //cout << "\t Follower: " << "" <<input.followers[id-1].size() ;
            //for(auto f : input.followers[id-1]){
                //cout << f << " ";
            //}
            //cout << endl;
        //}
    //}
    }

};




int main(){
    srand(time(0));
    //Input p;      `
    string filename = "dataIM5.txt";
    //Tao1File("dataIM9.txt");
    //TaoNhieuFile();
    //printData(filename);
    Solution s = Solution(filename);
    //s.setLeft();
    //s.firstSolution();
    //cout <<"First Solution " << endl;
    //s.printSolution();
    //s.clearSolution();
    //s.firstHeristic();
    //cout <<"FIRST HERISTIC !!" << endl;
    //s.printSolution();
    //s.clearSolution();
    auto start = chrono::high_resolution_clock::now();
    s.training();
    //s.printSolution();
    //s.clearSolution();
    s.printSolution();
    s.puting();
    s.LastSolution();
    //s.LocalSearch2();
    //s.FindBest();
    //s.Swap_1n1(0);
    //s.fillSolution();
    s.printSolution();
    //p.Display();
    //s.GoodHeristic(s.left);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Total time : " << duration.count() << " milliseconds" << endl;
}