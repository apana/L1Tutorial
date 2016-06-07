L1Tutorial
==========

#### L1Menu DPG package for menu-making
<pre><code>
git clone -b 2016-Tune https://github.com/cms-l1-dpg/L1Menu.git L1TriggerDPG/L1Menu
cd L1TriggerDPG/L1Menu
scramv1 b -j 9
cd macros
make -j 9
</code></pre>

#### HATS L1 Tutorial
<pre><code>
git clone git@github.com:apana/L1HATS.git
cd L1HATS/L1HATSexercise/macros
make
</code></pre>

### Run the example and plot the rates
<pre><code>
rateExample -n 1000000 -b 1824 -o Rates.root -l run274284.list
root Rates.root
</code></pre>
