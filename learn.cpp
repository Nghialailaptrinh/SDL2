 #include <iostream>

using namespace std;

int  tim_nhi_phan(int n,int l,int r,int a[100]){
if(l>r)return -1;
int m=l+(r-l)/2;
if(a[m]==n)return m;
else if(a[m]<n)return tim_nhi_phan(n,m+1,r,a);
else return tim_nhi_phan (n,l,m-1,a);
}

int main(){
int n;cin>>n;
int m;cin>>m;
int a[m];
for( int i=0;i<m;i++)cin>>a[i];
cout<<tim_nhi_phan(n,0,m-1,a);


}
