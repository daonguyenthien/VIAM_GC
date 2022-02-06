//Bien vao waypointList;
//Bien ra WP_out;
#define Earth_radius 6371000
#include "TSP_GA.h"

void TSP_GA::TSP_GA_Run(QList<QGeoCoordinate> waypointList)
{
    pop.N_WP = waypointList.length()-2;

    //diem bat dau va ket thuc la co dinh
    ConverttoXY(waypointList.value(0).latitude(),waypointList.value(0).longitude(),base_latlng.x,base_latlng.y,InitialPoint.x,InitialPoint.y);
    ConverttoXY(waypointList.value(waypointList.length()-1).latitude(),waypointList.value(waypointList.length()-1).longitude(),base_latlng.x,base_latlng.y,TerminalPoint.x,TerminalPoint.y);

    //Cac diem giua
    for(size_t i=0; i < pop.N_WP; i++){
        ConverttoXY(waypointList.value(i+1).latitude(),waypointList.value(i+1).longitude(), base_latlng.x, base_latlng.y,
        waypoints[i].x, waypoints[i].y);
    }

    //Khoi tao quan the
    Init_Population();
    bestfit.push_back(pop.fitness[pop.POPSIZE-1]);
    for (size_t generation = 1; generation < max_generation; generation++)
    {

        Select_Linear_Ranking();    //Chon loc sap hang tuyen tinh
        Crossover();                //Giao phoi
        Mutate_Uniform();           //Dot bien don boi
        Cal_Fitness();              //Tinh gia tri fitness = chieu dai quang duong

        bestfit.push_back(pop.fitness[pop.POPSIZE-1]);
        qDebug()<<generation<<" : "<<bestfit[generation];
        if (bestfit[generation] == bestfit[generation-1])
            stall_generation++;
        else
            stall_generation = 0;
        if(stall_generation == max_stall_generation)
            break;
    }


    Point tmpWP_out[pop.N_WP+2];
    tmpWP_out[0] = InitialPoint;
    for (int i = 0; i < pop.N_WP; i++)
        tmpWP_out[i+1] = waypoints[pop.individual[pop.POPSIZE-1][i]];
    tmpWP_out[pop.N_WP+1] = TerminalPoint;

    for (int i = 0; i < pop.N_WP+2; i++){
        Point tmpWaypoint;
        ConverttoLatLong(tmpWP_out[i].x, tmpWP_out[i].y, base_latlng.x, base_latlng.y, tmpWaypoint.x, tmpWaypoint.y);
        WP_out.append(tmpWaypoint);
    }

}

void TSP_GA::Init_Population()
{
    int r[pop.N_WP];

    for (int i = 0;i<pop.N_WP;i++)
        r[i]=i;
    for (int i=0;i<pop.POPSIZE;i++)    //Hoan vi ngau nhien cac diem waypoints
    {
        if (i!=0)
            ShuffleArray(r,pop.N_WP);
        for(int j=0;j<pop.N_WP;j++)
            pop.individual[i][j] = r[j];
    }
    Cal_Fitness();
}

void TSP_GA::Select_Linear_Ranking()
{
    int idx[pop.POPSIZE];
    double p[pop.POPSIZE]; //Phan bo xac suat
    double s[pop.POPSIZE+1];
    s[0] = 0;
    double eta = pop.eta;
    vector<int> V(pop.POPSIZE);
    std::iota(V.begin(),V.end(),0); //Initializing
    sort(V.begin(),V.end(), [&](int i,int j){return pop.fitness[i]>pop.fitness[j];} );
    int k=0;
    for (auto i = V.begin(); i != V.end(); ++i) //xu ly de luu lai gia tri index truoc do
    {
        idx[k] = *i;
        k ++;
    }                                                         //luu lai vi tri truoc khi chon loc
    sort(pop.fitness,pop.fitness+pop.POPSIZE,greater<int>()); //sap xep fitness theo thu tu giam dan

    for(int k=0;k<pop.POPSIZE;k++)
    {
        p[k] = (eta+2*(1-eta)*k/(pop.POPSIZE-1))/pop.POPSIZE;
        s[k+1] = s[k] + p[k];
    }
    parent = pop;

    for(int k=0;k<pop.POPSIZE;k++)
    {
        if (k == pop.POPSIZE-1)
            for(int j=0;j<pop.N_WP;j++)
                parent.individual[k][j] = pop.individual[idx[k]][j];    //giu lai ca the tot nhat
        else
        {
            int index = ceilingLinearSearch(s,pop.POPSIZE+1,rand() / (RAND_MAX + 1.0));
            for(int j=0;j<pop.N_WP;j++)
                parent.individual[k][j] = pop.individual[idx[index]][j];
        }

    }
}

void TSP_GA::Crossover()
{
    child = parent;
        for(int p1=0;p1<pop.POPSIZE;p1++)
            if(p1==pop.POPSIZE-1)   //giu lai ca the tot nhat
                for(int j=0;j<pop.N_WP;j++)
                    child.individual[p1][j] = parent.individual[p1][j];
            else
            {
                int p2=p1;
                while(p2==p1)
                    p2 = (rand() % pop.POPSIZE);    //dam bao p2 khac p1

                //Dieu kien lai ghep giua p1 va p2
                if (pop.cross_prob>rand() / (RAND_MAX + 1.0))
                {
                    int cross_point = rand() % pop.N_WP;
                    for(int j=0;j<cross_point;j++)
                        child.individual[p1][j] = parent.individual[p1][j]; //lay phan dau cua p1
                    for(int j=cross_point;j<pop.N_WP;j++)
                        child.individual[p1][j] = parent.individual[p2][j]; //va phan cuoi cua p2
                }
                //giai quyet nhung chromsome trung lap
                int cnt[pop.N_WP] = {};
                int coincide_point[pop.N_WP] = {};
                int miss_chromosome[pop.N_WP] = {};
                //tim cac vi tri trung lap. Vi du A = {1,2,3,3,4,3} => A[2] = A[3] = A[5] = 3 => can thay the A[3] va A[5]
                int cnt_coincide = 0;
                //i la vi tri, j la gia tri
                for(int i=0;i<pop.N_WP;i++)
                {
                    for(int j=0;j<i;j++)
                        if (child.individual[p1][i] == child.individual[p1][j])
                        {
                            coincide_point[cnt_coincide] = i;
                            cnt_coincide ++;
                            break;
                        }
                }
                //tim cac gia tri bi mat. Vi du A = {0,2,3,4,5,5} => thieu gia tri 1 va thua gia tri A[5] = 5 => Can thay A[5] = 1;
                int cnt_miss = 0;
                for(int i=0;i<pop.N_WP;i++)
                    for(int j=0;j<pop.N_WP;j++)
                        if (child.individual[p1][i] == j)
                            cnt[j]++;

                for(int i=0;i<pop.N_WP;i++)
                    if (cnt[i] == 0)
                    {
                        miss_chromosome[cnt_miss] = i;
                        cnt_miss++;
                    }

                //the cac gia tri bi mat vao cac o bi trung
                for(int i=0;i<cnt_miss;i++)
                    child.individual[p1][coincide_point[i]] = miss_chromosome[i];
            }
}

void TSP_GA::Mutate_Uniform()
{
    pop = child;
    for(int pop_index = 0;pop_index<pop.POPSIZE;pop_index++)
        if(pop_index != pop.POPSIZE-1)  //khong dong toi ca the fitness tot nhat
            //hoan doi 2 chromosome bat ky
            {
                int chromosome1 = rand() % pop.N_WP;
                int chromosome2 = rand() % pop.N_WP;
                pop.individual[pop_index][chromosome1] = child.individual[pop_index][chromosome2];
                pop.individual[pop_index][chromosome2] = child.individual[pop_index][chromosome1];
            }
}

void TSP_GA::Cal_Fitness()
{
//    //Straght Planning
//    for(int i=0;i<pop.POPSIZE;i++)
//    {
//        pop.fitness[i] = hypot(waypoints[pop.individual[i][0]].x-InitialPoint.x,waypoints[pop.individual[i][0]].y-InitialPoint.y);
//        for(int j=0;j<pop.N_WP-1;j++)
//            pop.fitness[i] += hypot(waypoints[pop.individual[i][j+1]].x-waypoints[pop.individual[i][j]].x,waypoints[pop.individual[i][j+1]].y-waypoints[pop.individual[i][j]].y);
//        pop.fitness[i] += hypot(waypoints[pop.individual[i][pop.N_WP-1]].x-TerminalPoint.x,waypoints[pop.individual[i][pop.N_WP-1]].y-TerminalPoint.y);
//    }

    //Dubins Planning
    for(int i=0;i<pop.POPSIZE;i++)
    {
      PathPlanningDubins *dubinsObjects = new PathPlanningDubins();
      dubinsObjects->numIn = (size_t)pop.N_WP+2;
      dubinsObjects->num_Vc = this->num_Vc;
      for(int i = 0;i<num_Vc;i++)
      {
          ConverttoXY(Obs_lat[i],Obs_lng[i],base_latlng.x,base_latlng.y,dubinsObjects->Obs_x[i],dubinsObjects->Obs_y[i]);
        dubinsObjects->Obs_R[i] = Obs_R[i];
      }
      dubinsObjects->wp_xy_in.resize(pop.N_WP+2);
      dubinsObjects->wp_xy_in[0] = InitialPoint;
      dubinsObjects->wp_xy_in[pop.N_WP+1] = TerminalPoint;
      for(int j=1;j<pop.N_WP+1;j++)
      {
          dubinsObjects->wp_xy_in[j] = waypoints[pop.individual[i][j-1]];
      }
      dubinsObjects->F_Dubins_Path_Planning();
      pop.fitness[i] = dubinsObjects->path_length;
    }
}

void TSP_GA::ShuffleArray(int* arr, int n)
{
    int minPosition;
    int maxPosition = n - 1;
    int swapPosition;

    int i = 0;
    while (i < n - 1)
    {
        minPosition = i + 1;
        swapPosition = rand() % (maxPosition - minPosition + 1) + minPosition;
        Swap(&arr[i], &arr[swapPosition]);
        i++;
    }
}

void TSP_GA::Swap(int* number_1, int* number_2)
{
    int temp = *number_1;
    *number_1 = *number_2;
    *number_2 = temp;
}

int TSP_GA::ceilingLinearSearch(double arr[], int n, double x)
{
    for (int i = 0; i < n; i++) {
        if (arr[i] > x)
            return i-1;
    }
    return -1;
}
