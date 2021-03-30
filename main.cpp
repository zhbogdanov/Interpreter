#include "executer.hpp"

class Interpretator {
  Parser   pars;
  Executer E;
  public:
  Interpretator(const char *program): pars(program) {}
  void interpretation();
};
 
void Interpretator::interpretation() {
  pars.analyze();
  E.execute(pars.poliz);
}
 
int main() {
  try {
    Interpretator I("prog2.txt");
    I.interpretation();
    return 0;
  }
  catch (char c) {
    cout << "unexpected symbol " << c << endl;
    return 1;
  }
  catch (Lex l) {
    cout << "unexpected lexeme" << l << endl;
    return 1;
  }
  catch (const char *source) {
    cout << source << endl;
    return 1;
  }
}
