
/*
DiscreteSampler.cpp - This file is part of BayesTyper (https://github.com/bioinformatics-centre/BayesTyper)


The MIT License (MIT)

Copyright (c) 2016 Jonas Andreas Sibbesen and Lasse Maretty

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#include <limits>

#include "Utils.hpp"
#include "DiscreteSampler.hpp"

DiscreteSampler::DiscreteSampler() {}

DiscreteSampler::DiscreteSampler(uint size_guess) {

	cum_probs.reserve(size_guess);
}

DiscreteSampler::DiscreteSampler(vector<double> & cum_probs_in) {

	cum_probs = cum_probs_in;
}

void DiscreteSampler::addOutcome(double prob) {

	assert(std::isfinite(prob));

	if (cum_probs.empty()) {

		cum_probs.push_back(prob);
	
	} else {

		cum_probs.push_back(prob + cum_probs.back());
	}
}

uint DiscreteSampler::sample(mt19937 * prng) const {

	double random01_sample_scaled = generate_canonical<double,std::numeric_limits<double>::digits>(*prng) * cum_probs.back();
	
	return search(random01_sample_scaled);
}

uint DiscreteSampler::search(double random01_sample_scaled) const {

	assert(!(cum_probs.empty()));
	assert(std::isfinite(random01_sample_scaled));

	if (cum_probs.size() > 1) {

		auto sample_iter = upper_bound(cum_probs.begin(), cum_probs.end(), random01_sample_scaled);
		assert(sample_iter != cum_probs.end());

		uint sample_idx = sample_iter - cum_probs.begin();
		assert(sample_idx >= 0);

		return sample_idx;

	} else {

		return 0;
	}
}

double DiscreteSampler::probSum() const {

	assert(!(cum_probs.empty()));
	return cum_probs.back();
}

void DiscreteSampler::clear() {

	cum_probs.clear();
}

LogDiscreteSampler::LogDiscreteSampler() {}

LogDiscreteSampler::LogDiscreteSampler(uint size_guess) : DiscreteSampler(size_guess) {}

LogDiscreteSampler::LogDiscreteSampler(vector<double> & cum_log_probs_in) : DiscreteSampler(cum_log_probs_in) {}

void LogDiscreteSampler::addOutcome(double log_prob) {

	assert(std::isfinite(log_prob));

	if (cum_probs.empty()) {

		cum_probs.push_back(log_prob);

	} else {

		cum_probs.push_back(Utils::logAddition(log_prob, cum_probs.back())); 
	}
}

uint LogDiscreteSampler::sample(mt19937 * prng) const {

	double random01_log_sample_scaled = log(generate_canonical<double,std::numeric_limits<double>::digits>(*prng)) + cum_probs.back();

	return search(random01_log_sample_scaled);
}

