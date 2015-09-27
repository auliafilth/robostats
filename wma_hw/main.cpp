#include <iostream>
#include <fstream>

#include "Nature.h"
#include "Learner.h"
#include "StochasticNature.h"
#include "DeterministicNature.h"
#include "AdversarialNature.h"

using namespace std;

/*
 * This is an online learning Prediction with Expert Advice Model (PWEA).
 * It used to simulate the Weighted Majority Algorithm.
 *
 * Outline of how the code should run:
 * 1. Nature send observation
 * 2. Learner receives observation, asks each Expert to make a prediction.
 * 3. Nature determines the label.
 *    For adversarial case, the nature gets to see the weight vector and prediction
 *    for each Expert (but not directly the final prediction made by the learner).
 * 4. Learner makes final prediction and the true label is revealed.
 * 5. Learner suffers loss and updates Expert weight.
 * 6. Repeat.
 * 
 * Total Rounds = 100.
 */

#define WMA  0
#define RWMA 1
enum NAT_TYPE { DETER, STOCH, ADVER };

/*************************************************************
 * Test Stochastic Nature class.
 */
void test_stochastic() {
    // test stochastic nature
    StochasticNature s_nature = StochasticNature();
    for(int i=0; i<20; i++)
        cout << s_nature.get_label(0) << endl;
}

/*************************************************************
 * Test Deterministic Nature class.
 */
void test_deterministic() {
    // test deterministic nature
    DeterministicNature d_nature = DeterministicNature();
    for(int i=0; i<50; i++)
        cout << i << " " << d_nature.is_prime(i) << endl;
}

/*************************************************************
 * Print regret/loss to file.
 */
void print_to_file(int round, std::ofstream &myfile, Learner my_learner) {

    cout << round << ", ";
    myfile << round << ", ";

    cout << my_learner.get_learner_loss() << ", ";
    myfile << my_learner.get_learner_loss() << ", ";
    cout << my_learner.get_regret() << ", ";
    myfile << my_learner.get_regret() << ", ";

    std::vector<int> expert_loss = my_learner.get_expert_loss();
    for (int i=0; i<expert_loss.size(); i++) {
        cout << expert_loss[i] << ", ";
        myfile << expert_loss[i] << ", ";
    }
    cout << endl;
    myfile << endl;
}

/*************************************************************
 * MAIN
 */
int main() {
    float eta = 0.5;
    int num_rounds = 1000;
    NAT_TYPE nat = DETER;
    int learner_type = RWMA;

    ofstream myfile;
    myfile.open("loss.txt");

    Nature* my_nature;  // = new Nature(num_rounds);
    Learner my_learner = Learner();
    my_learner.set_eta(eta);

    switch(nat) {
        case ADVER: 
            my_nature = new AdversarialNature(num_rounds);
        break;
        case DETER: 
            my_nature = new DeterministicNature(num_rounds);
        break;
        case STOCH:
            my_nature = new StochasticNature(num_rounds);
        break;
    }

    // tests
    //cout << my_learner.get_num_experts() << endl;
    //cout << my_learner.get_num_weights() << endl;
    //my_learner.print_experts();
    //my_learner.print_weights();
    //test_stochastic();
    //test_deterministic();

    for (int round=0; round<num_rounds; round++) {
        int nat_label;
        cout << "-------------------------------------------------" << endl;
        cout << "ROUND " << round << endl;
        // Learner get observation from Nature
        my_learner.set_observations(my_nature->get_observations(round));
        // Learner ask Experts to make prediction
        my_learner.make_predictions();

        // Calculate Nature's label
        if (nat == ADVER) {
            nat_label = my_nature->get_label(round, 
                                            my_learner.get_e_predictions(), 
                                            my_learner.get_weights() );
            cout << "ADVER label: " << nat_label << endl;
        } else if (nat == DETER) {
            nat_label = my_nature->get_label(round);
            cout << "DETER label: " << nat_label << endl;
        } else {
            nat_label = my_nature->get_label(round);
            cout << "STOCH label: " << nat_label << endl;
        }

        // Calculate Learner's label
        if (my_learner.calculate_label(learner_type)) {
            cout << "ERROR calculating label" << endl;
            return 1;
        }
        cout << "LEARNER label: " << my_learner.get_label() << endl;

        // Update weights
        my_learner.update_weights(nat_label);

        // Calculate loss 
        my_learner.calculate_loss(nat_label);

        // Print regret/loss to file
        print_to_file(round, myfile, my_learner);
    }

    myfile.close();

    delete my_nature;

    return 0; 

/*
    AdversarialNature a_nat = AdversarialNature(num_rounds);
    for (int i=0; i<num_rounds; i++) {
        cout << a_nat.get_observations(i)[0] << endl;
    }

    int round = 0;
    int a_label;
    // Learner get observation from Nature
    my_learner.set_observations(a_nat.get_observations(round));
    // Learner ask Experts to make prediction
    my_learner.make_predictions();
    a_label = a_nat.get_label(round, my_learner.get_e_predictions(),
                             my_learner.get_weights());
    cout << "adv label: " << a_label << endl;
    if (my_learner.calculate_label()) {
        cout << "ERROR calculating label" << endl;
        return 1;
    }
    cout << "learner label: " << my_learner.get_label() << endl;
*/
} 
