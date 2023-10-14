int main() {
    try {
        throw 1;
    } catch (int &b) {
        return 0;
    }
    return 1;
}