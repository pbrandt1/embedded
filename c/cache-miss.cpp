int a[320000000];
int sum;

int main() {
  for (int i = 0; i < 10000000; i++) {
    sum = sum + a[i * 32];
  }
}
